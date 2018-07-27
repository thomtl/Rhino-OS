#include "paging.h"

extern uint32_t placement_address;
uintptr_t page_directory;
uintptr_t tab_addr;
extern void tlb_flush();
extern heap_t* kheap;


void page_fault(registers_t* regs){
	unsigned int err_pos;
	__asm__ __volatile__ ("mov %%cr2, %0" : "=r" (err_pos));

	kprint_err("Page fault occurred at ");
  char address[35] = "";
  hex_to_ascii(err_pos, address);
  kprint_err(address);

	kprint_err("\nReasons:");

	int no_page = regs->err_code & 1;
	int rw = regs->err_code & 2;
	int um = regs->err_code & 4;
	int re = regs->err_code & 8;
	int dc = regs->err_code & 16;

	if(dc)		kprint_err(" (Instruction decode error) ");
	if(!no_page)	kprint_err(" (No page present) ");
	if(um)		kprint_err(" (in user mode) ");
	if(rw)		kprint_err(" (Write permissions) ");
	if(re)		kprint_err(" (RE) ");

	kprint_err("\n");
  CLI();
  while(1);
}

void init_mm_paging(){
	//char buf[25] = "";
  register_interrupt_handler(14, page_fault);

	page_directory = (uintptr_t)alloc_frame();
	tab_addr = (uintptr_t)alloc_frame();
	unsigned int pde = 0;
	for(pde = 0; pde < MM_PAGE_COMMON_SIZE; pde++){
		((uintptr_t*)page_directory)[pde] = 0 | 2;
		((uintptr_t*)tab_addr)[pde] = 0 | 2;
	}

	//for(uint32_t i = 0; i < placement_address; i += MM_PAGE_S){
		//map_phys_virt(page_directory, 0x0, phys_to_virt(0x0));
		//map_phys_virt(page_directory, MM_PAGE_S, phys_to_virt(MM_PAGE_S));
		//map_phys_virt(page_directory, MM_PAGE_S * 2, phys_to_virt(MM_PAGE_S * 2));
		//map_phys_virt(page_directory, MM_PAGE_S * 3, phys_to_virt(MM_PAGE_S * 3));
		//map_phys_virt(page_directory, MM_PAGE_S * 4, phys_to_virt(MM_PAGE_S * 4));
	//}
	for(uint32_t i = 0; i < MM_PAGE_S * 8; i += MM_PAGE_S){
		map_phys_virt(page_directory, i, phys_to_virt(i));
	}

	for(uint32_t i = MM_PAGE_S * 8; i < MM_PAGE_S * 8 + KHEAP_INITIAL_SIZE; i+= MM_PAGE_S) {
		map_phys_virt(page_directory, i, KHEAP_START);
	}
//__asm__ ("cli; hlt");



	/*uintptr_t address = 0x0;
	unsigned int pidx = 0;
	for(pidx = KERNEL_VBASE >> 22; pidx < (KERNEL_VBASE >> 22) + 8; pidx++){
		uintptr_t page_table = (uintptr_t)alloc_frame();

		unsigned int pte;
		for(pte = 0; pte < MM_PAGE_COMMON_SIZE; pte++){
			((uint32_t*)page_table)[pte] = address | 3;
			address += 0x1000;
		}

		((uintptr_t*)page_directory)[pidx] = virt_to_phys(page_table) | 3;
	}*/

	__asm__ __volatile__ ("mov %0, %%cr3":: "r"(virt_to_phys(page_directory)));

	/* clear 4MB page bit since we're switching to 4KB pages */
	uint32_t cr4;
	__asm__ __volatile__("mov %%cr4, %0": "=r" (cr4));
	cr4 &= ~(0x00000010);
	__asm__ __volatile__("mov %0, %%cr4":: "r" (cr4));

	/* read cr0, set paging bit, write it back */
	uint32_t cr0;
	__asm__ __volatile__("mov %%cr0, %0": "=r" (cr0));
	cr0 |= 0x80000000;
	__asm__ __volatile__("mov %0, %%cr0":: "r" (cr0));
	kheap = create_heap(KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, 0xDFFFF000, 0, 0);
}

void switch_dir(uintptr_t new_dir){
	__asm__ __volatile__ ("mov %0, %%cr3":: "r"(virt_to_phys(new_dir)));
}

void map_phys_virt(uintptr_t page_directory, uintptr_t phys, uintptr_t virt){
	uintptr_t address = phys;
	unsigned int pidx = 0;
	for(pidx = virt >> 22; pidx < (virt >> 22) + 1; pidx++){
		uintptr_t page_table = (uintptr_t)alloc_frame();

		unsigned int pte;
		for(pte = 0; pte < MM_PAGE_COMMON_SIZE; pte++){
			((uint32_t*)page_table)[pte] = address | 3;
			address += 0x1000;
		}

		((uintptr_t*)page_directory)[pidx] = virt_to_phys(page_table) | 3;
		((uintptr_t*)tab_addr)[pidx] = page_table;
	}
	tlb_flush();
}

void unmap_phys_virt(uintptr_t page_directory, uintptr_t virt){
	uint32_t pidx = virt >> 22;
	uintptr_t page_table = ((uintptr_t*)page_directory)[pidx];
	((uintptr_t*)page_directory)[pidx] = 0 | 2;
	((uintptr_t*)tab_addr)[pidx] = 0 | 2;
	free_frame((void*)phys_to_virt(page_table));
	tlb_flush();
}

uintptr_t phys_to_virt(uintptr_t phys){
	return phys + KERNEL_VBASE;
}

uintptr_t virt_to_phys(uintptr_t virt){
	return virt - KERNEL_VBASE;
}

/*static uintptr_t clone_tab(uintptr_t dir){
	uintptr_t ret = (uintptr_t)kmalloc_a(0x1000);
	for(uint32_t i = 0; i < MM_PAGE_COMMON_SIZE; i++){
		//((uintptr_t*)ret)[i] = ((uintptr_t*)dir)[i];
		memcpy(&(((uintptr_t*)ret)[i]), &(((uintptr_t*)dir)[i]), sizeof(uintptr_t));
	}
	//memcpy((uintptr_t*)ret, (uintptr_t*)dir, sizeof(uintptr_t) * MM_PAGE_COMMON_SIZE);
	return ret;
}*/

//TODO: some kind of page structure that allows for tab_addr, now only main dir is cloneble
uintptr_t clone_dir(uintptr_t dir){
	uintptr_t ret = (uintptr_t)alloc_frame();

	/*for(uint32_t i = 0; i < MM_PAGE_COMMON_SIZE; i++){
		uint32_t t = ((uintptr_t*)dir)[i];
		BIT_CLEAR(t, 0);
		BIT_CLEAR(t, 1);
		BIT_CLEAR(t, 2);
		BIT_CLEAR(t, 5);
		t = phys_to_virt(t);
		if(((uintptr_t*)dir)[i] == (0 | 2)){
			((uintptr_t*)ret)[i] = (0 | 2);
		}
		if((((uintptr_t*)tab_addr)[i] != (0 | 2)))
		{
			uintptr_t a = clone_tab(t);
			((uintptr_t*)ret)[i] = ((a - 0xD0000000)| 3);
		}
	}*/
	for(uint32_t i = 0; i < MM_PAGE_COMMON_SIZE; i++){
		memcpy(&(((uintptr_t*)ret)[i]), &(((uintptr_t*)dir)[i]), sizeof(uintptr_t));
	}
	return ret;
}