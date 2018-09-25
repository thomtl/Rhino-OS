#include <rhino/mm/vmm.h>
extern void tlb_flush();
pdirectory* vmm_cur_directory = 0;
pdirectory* kernel_directory = 0;
void vmm_pt_entry_add_attrib(pt_entry* e, uint32_t attrib){
    *e |= attrib;
}
void vmm_pt_entry_del_attrib(pt_entry* e, uint32_t attrib){
    *e &= ~attrib;
}
void vmm_pt_entry_set_frame(pt_entry* e, void* frame){

    *e = (*e & ~PHI_PTE_FRAME) | (uint32_t)frame;

}
bool vmm_pt_entry_is_present(pt_entry e){
    return e & PHI_PTE_PRESENT;
}
bool vmm_pt_entry_is_writable(pt_entry e){
    return e & PHI_PTE_WRITABLE;
}
void* vmm_pt_entry_pfn(pt_entry e){
    return (void*)(e & PHI_PTE_FRAME);
}

void vmm_pd_entry_add_attrib(pd_entry* e, uint32_t attrib){
    *e |= attrib;
}
void vmm_pd_entry_del_attrib(pd_entry* e, uint32_t attrib){
    *e &= ~attrib;
}
void vmm_pd_entry_set_frame(pd_entry* e, void* frame){
    *e = (*e & ~PHI_PDE_FRAME) | (uint32_t)frame;
}
bool vmm_pd_entry_is_present(pd_entry e){
    return e & PHI_PDE_PRESENT;
}
bool vmm_pd_entry_is_user(pd_entry e){
    return e & PHI_PDE_USER;
}
bool vmm_pd_entry_is_writable(pd_entry e){
    return e & PHI_PDE_WRITABLE;
}
bool vmm_pd_entry_is_4mb(pd_entry e){
    return e & PHI_PDE_4MB;
}
void* vmm_pd_entry_pfn(pd_entry e){
    return (void*)(e & PHI_PDE_FRAME);
}

void vmm_pd_entry_enable_global(pd_entry e){
    e |= PHI_PDE_LV4_GLOBAL;
}


bool vmm_alloc_page(pt_entry* e){
    void* p = (void*)((uint32_t)pmm_alloc_block() + KERNEL_VBASE);
    if(!p) return false;
    vmm_pt_entry_set_frame(e, p);
    vmm_pt_entry_add_attrib(e, PHI_PTE_PRESENT);
    return true;
}

void vmm_free_page(pt_entry* e){
    void* p = vmm_pt_entry_pfn(*e);
    if(p) pmm_free_block(p);
    vmm_pt_entry_del_attrib(e, PHI_PTE_PRESENT);
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
    if(!dir) return false;

    vmm_cur_directory = dir;
    //TODO load dir into cr3
    uint32_t phys = (uint32_t)((uint32_t)dir - (uint32_t)KERNEL_VBASE);
    asm("mov %0, %%cr3":: "r"(phys));
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


    if((*e & PHI_PTE_PRESENT) != PHI_PTE_PRESENT){
        ptable* table = (ptable*)((uint32_t)pmm_alloc_block() + KERNEL_VBASE);
        if(!table) return;
        //if((uint32_t)table > 0xC03FFFFF) vmm_map_page((void*)((uint32_t)table - (uint32_t)KERNEL_VBASE), (void*)(table));
        vmm_ptable_clear(table);


        memset(table, 0, sizeof(ptable));

        pd_entry* entry = &pageDirectory->m_entries[PAGE_DIRECTORY_INDEX((uint32_t)virt)];
        vmm_pd_entry_add_attrib(entry, PHI_PDE_PRESENT);
        vmm_pd_entry_add_attrib(entry, PHI_PDE_WRITABLE);
        if(user) vmm_pd_entry_add_attrib(entry, PHI_PDE_USER);
        void* fr = (void*)((uint32_t)table - (uint32_t)KERNEL_VBASE);

        vmm_pd_entry_set_frame(entry, fr);


    }

    ptable* table = (ptable*)PAGE_GET_PHYSICAL_ADDRESS(e);

    pt_entry* page = &table->m_entries[PAGE_TABLE_INDEX((uint32_t)virt)];

    pt_entry* pagv = (pt_entry*)((uint32_t)page + (uint32_t)KERNEL_VBASE);// convert page into virtual from physical

    vmm_pt_entry_add_attrib(pagv, PHI_PTE_WRITABLE);
    vmm_pt_entry_set_frame(pagv, (void*)phys);
    vmm_pt_entry_add_attrib(pagv, PHI_PTE_PRESENT);
    if(user) vmm_pt_entry_add_attrib(pagv, PHI_PTE_USER);
    tlb_flush();
}

void vmm_ptable_clear(ptable* tab){
    memset(tab, 0, sizeof(ptable));
}

void vmm_pdirectory_clear(pdirectory* dir){
    memset(dir, 0, sizeof(pdirectory));
}

bool init_vmm(){
    /*ptable* table = (ptable*)((uint32_t)pmm_alloc_block() + KERNEL_VBASE);
    if(!table) return;
    vmm_ptable_clear(table);
	for (int i=0, frame=0x000000, virt=0xc0000000; i<1024; i++, frame+=4096, virt+=4096) {
		pt_entry page=0;
		vmm_pt_entry_add_attrib (&page, PHI_PTE_PRESENT);
        vmm_pt_entry_add_attrib(&page, PHI_PTE_WRITABLE);
		vmm_pt_entry_set_frame (&page, (void*)frame);
		table->m_entries [PAGE_TABLE_INDEX (virt) ] = page;
	}*/

    pdirectory* dir = (pdirectory*)((uint32_t)pmm_alloc_block() + KERNEL_VBASE);
		kernel_directory = dir;
    if(!dir) return false;

    vmm_pdirectory_clear(dir);

    vmm_cur_directory = dir;

    /*pd_entry* entry = &dir->m_entries [PAGE_DIRECTORY_INDEX (0xc0000000) ];
    vmm_pd_entry_add_attrib(entry, PHI_PDE_PRESENT);
    vmm_pd_entry_add_attrib(entry, PHI_PDE_WRITABLE);
    uint32_t p = (uint32_t)((uint32_t)table - (uint32_t)KERNEL_VBASE);
    vmm_pd_entry_set_frame(entry, (void*)p);*/

    for (int i=0, frame=0x000000, virt=0xc0000000; i<(1024 * 16); i++, frame+=4096, virt+=4096) {

		vmm_map_page((void*)frame, (void*)virt, 0);

	}

    vmm_switch_pdirectory(dir);
    return true;
}

pdirectory* vmm_clone_dir(pdirectory* dir){
	uint32_t ret = (uint32_t)((uint32_t)pmm_alloc_block() + KERNEL_VBASE);
  //vmm_map_page((void*)((uint32_t)ret - (uint32_t)KERNEL_VBASE), (void*)(ret));
	for(uint32_t i = 0; i < 1024; i++){
		memcpy(&(((uintptr_t*)ret)[i]), &(((uintptr_t*)dir)[i]), sizeof(uintptr_t));
	}

	return (pdirectory*)ret;
}
