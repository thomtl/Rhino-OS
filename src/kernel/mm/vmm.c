#include <rhino/mm/vmm.h>

pdirectory* vmm_cur_directory = 0;
pdirectory* kernel_directory = 0;

void vmm_pt_entry_add_attrib(pt_entry* e, uint32_t attrib){
    *e |= attrib;
}
void vmm_pt_entry_del_attrib(pt_entry* e, uint32_t attrib){
    *e &= ~attrib;
}
void vmm_pt_entry_set_frame(pt_entry* e, void* frame){

    *e = (*e & ~RHINO_PTE_FRAME) | (uint32_t)frame;

}
bool vmm_pt_entry_is_present(pt_entry e){
    return e & RHINO_PTE_PRESENT;
}
bool vmm_pt_entry_is_writable(pt_entry e){
    return e & RHINO_PTE_WRITABLE;
}
void* vmm_pt_entry_pfn(pt_entry e){
    return (void*)(e & RHINO_PTE_FRAME);
}

void vmm_pd_entry_add_attrib(pd_entry* e, uint32_t attrib){
    *e |= attrib;
}
void vmm_pd_entry_del_attrib(pd_entry* e, uint32_t attrib){
    *e &= ~attrib;
}
void vmm_pd_entry_set_frame(pd_entry* e, void* frame){
    *e = (*e & ~RHINO_PDE_FRAME) | (uint32_t)frame;
}
bool vmm_pd_entry_is_present(pd_entry e){
    return e & RHINO_PDE_PRESENT;
}
bool vmm_pd_entry_is_user(pd_entry e){
    return e & RHINO_PDE_USER;
}
bool vmm_pd_entry_is_writable(pd_entry e){
    return e & RHINO_PDE_WRITABLE;
}
bool vmm_pd_entry_is_4mb(pd_entry e){
    return e & RHINO_PDE_4MB;
}
void* vmm_pd_entry_pfn(pd_entry e){
    return (void*)(e & RHINO_PDE_FRAME);
}

void vmm_pd_entry_enable_global(pd_entry e){
    e |= RHINO_PDE_LV4_GLOBAL;
}


bool vmm_alloc_page(pt_entry* e){
    void* p = (void*)((uint32_t)pmm_alloc_block() + KERNEL_VBASE);
    if(!p) return false;
    vmm_pt_entry_set_frame(e, p);
    vmm_pt_entry_add_attrib(e, RHINO_PTE_PRESENT);
    return true;
}

void vmm_free_page(pt_entry* e){
    void* p = vmm_pt_entry_pfn(*e);
    if(p) pmm_free_block(p);
    vmm_pt_entry_del_attrib(e, RHINO_PTE_PRESENT);
}

pt_entry* vmm_ptable_lookup_entry(ptable* p, void* addr){
    if(p) return &p->m_entries[PAGE_TABLE_INDEX((uint32_t)addr)];
    return 0;
}
pd_entry* vmm_pdirectory_lookup_entry(pdirectory* p, void* addr){
    if(p) return &p->m_entries[PAGE_TABLE_INDEX((uint32_t)addr)];
    return 0;
}

bool vmm_switch_pdirectory(pdirectory* dir){
    if(!dir){
        return false;
    }

    vmm_cur_directory = dir;
    uint32_t phys = (uint32_t)((uint32_t)dir - (uint32_t)KERNEL_VBASE);
    asm("mov %0, %%cr3":: "r"(phys));
    //debug_log("[VMM]: CR3 Set\n");
    return true;
}
pdirectory* vmm_get_directory(){
    return vmm_cur_directory;
}

void vmm_flush_tlb_entry(void* addr){
    asm("invlpg (%0)":: "r"(addr));
}

void vmm_map_page(void* phys, void* virt, uint32_t user){
    pdirectory* pageDirectory = vmm_get_directory();

    pd_entry* e = &pageDirectory->m_entries[PAGE_DIRECTORY_INDEX((uint32_t)virt)];


    if((*e & RHINO_PTE_PRESENT) != RHINO_PTE_PRESENT){
        ptable* table = (ptable*)((uint32_t)pmm_alloc_block() + KERNEL_VBASE);
        if(!table) return;
        vmm_ptable_clear(table);


        memset(table, 0, sizeof(ptable));

        pd_entry* entry = &pageDirectory->m_entries[PAGE_DIRECTORY_INDEX((uint32_t)virt)];
        vmm_pd_entry_add_attrib(entry, RHINO_PDE_PRESENT);
        vmm_pd_entry_add_attrib(entry, RHINO_PDE_WRITABLE);
        if(user) vmm_pd_entry_add_attrib(entry, RHINO_PDE_USER);
        void* fr = (void*)((uint32_t)table - (uint32_t)KERNEL_VBASE);

        vmm_pd_entry_set_frame(entry, fr);


    }

    ptable* table = (ptable*)PAGE_GET_PHYSICAL_ADDRESS(e);

    pt_entry* page = &table->m_entries[PAGE_TABLE_INDEX((uint32_t)virt)];

    pt_entry* pagv = (pt_entry*)((uint32_t)page + (uint32_t)KERNEL_VBASE);

    vmm_pt_entry_add_attrib(pagv, RHINO_PTE_WRITABLE);
    vmm_pt_entry_set_frame(pagv, (void*)phys);
    vmm_pt_entry_add_attrib(pagv, RHINO_PTE_PRESENT);
    if(user) vmm_pt_entry_add_attrib(pagv, RHINO_PTE_USER);
    vmm_flush_tlb_entry(virt);
}

void vmm_unmap_page(void* virt){
    pdirectory* pageDirectory = vmm_get_directory();

    pd_entry* e = &pageDirectory->m_entries[PAGE_DIRECTORY_INDEX((uint32_t)virt)];

    ptable* table = (ptable*)PAGE_GET_PHYSICAL_ADDRESS(e);

    pt_entry* page = &table->m_entries[PAGE_TABLE_INDEX((uint32_t)virt)];

    pt_entry* pagv = (pt_entry*)((uint32_t)page + (uint32_t)KERNEL_VBASE);// convert page into virtual from physical

    vmm_pt_entry_del_attrib(pagv, RHINO_PTE_PRESENT);
    vmm_flush_tlb_entry(virt);
}

bool vmm_page_is_mapped(void* virt){
    pdirectory* pageDirectory = vmm_get_directory();

    pd_entry* e = &pageDirectory->m_entries[PAGE_DIRECTORY_INDEX((uint32_t)virt)];

    ptable* table = (ptable*)PAGE_GET_PHYSICAL_ADDRESS(e);

    pt_entry* page = &table->m_entries[PAGE_TABLE_INDEX((uint32_t)virt)];

    pt_entry* pagv = (pt_entry*)((uint32_t)page + (uint32_t)KERNEL_VBASE);// convert page into virtual from physical

    return vmm_pt_entry_is_present(*pagv);
}

void vmm_ptable_clear(ptable* tab){
    memset(tab, 0, sizeof(uint32_t) * 1024);
}

void vmm_pdirectory_clear(pdirectory* dir){
    memset(dir, 0, sizeof(uint32_t) * 1024);
}

bool init_vmm(){
    debug_log("[VMM]: Initializing VMM\n");
    pdirectory* dir = (pdirectory*)((uint32_t)pmm_alloc_block() + KERNEL_VBASE);
		kernel_directory = dir;
    if(!dir) return false;

    vmm_pdirectory_clear(dir);

    vmm_cur_directory = dir;

    for (int i=0, frame=0x000000, virt=0xc0000000; i<(1024 * 16); i++, frame+=4096, virt+=4096) {

		vmm_map_page((void*)frame, (void*)virt, 0);

	}

    vmm_switch_pdirectory(dir);
    debug_log("[VMM]: VMM Initialized\n");
    return true;
}

uint32_t vmm_clone_tab(pd_entry* pde){
    ptable* org = (ptable*)PAGE_GET_PHYSICAL_ADDRESS(pde);
    uint32_t ret = (uint32_t)((uint32_t)pmm_alloc_block() + KERNEL_VBASE);
    vmm_ptable_clear((ptable*)ret);
    for(uint32_t i = 0; i < 1024; i++){
        pt_entry* page = &((org->m_entries)[i]);
        pt_entry* pagv = (pt_entry*)((uint32_t)page + (uint32_t)KERNEL_VBASE);
        uint32_t* pte = (uint32_t*)ret + i;
        *pte = *pagv;
    }
    return (uint32_t)ret;
}

pdirectory* vmm_clone_dir(pdirectory* dir){
	uint32_t ret = (uint32_t)((uint32_t)pmm_alloc_block() + KERNEL_VBASE);
    vmm_pdirectory_clear((pdirectory*)ret);
	for(uint32_t i = 0; i < 1024; i++){
		//memcpy(&(((uintptr_t*)ret)[i]), &(((uintptr_t*)dir)[i]), sizeof(uintptr_t));
        if(vmm_pd_entry_is_present(dir->m_entries[i])){
            uint32_t* pde = (uint32_t*)ret + i;
            vmm_pd_entry_add_attrib(pde, RHINO_PDE_PRESENT);
            vmm_pd_entry_add_attrib(pde, RHINO_PDE_WRITABLE);
            if(vmm_pd_entry_is_user(dir->m_entries[i])) vmm_pd_entry_add_attrib(pde, RHINO_PDE_USER);
            vmm_pd_entry_set_frame(pde, (void*)(vmm_clone_tab(&(dir->m_entries[i])) - KERNEL_VBASE));
        }
	}
	return (pdirectory*)ret;
}

void vmm_free_dir(pdirectory* dir){
    for(uint32_t i = 0; i < 1024; i++){
        if(vmm_pd_entry_is_present(dir->m_entries[i])){
            void* tab = vmm_pd_entry_pfn(dir->m_entries[i]);
            pmm_free_block(tab);
        }
    }
    pmm_free_block((void*)((uint32_t)dir - (uint32_t)KERNEL_VBASE));
}