#include "paging.h"

extern uint32_t placement_address;

extern void tlb_flush();



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

	uintptr_t page_directory = (uintptr_t)alloc_frame();

	unsigned int pde = 0;
	for(pde = 0; pde < MM_PAGE_COMMON_SIZE; pde++){
		((uintptr_t*)page_directory)[pde] = 0 | 2;
	}

	uintptr_t address = 0x0;
	unsigned int pidx = 0;
	for(pidx = KERNEL_VBASE >> 22; pidx < (KERNEL_VBASE >> 22) + 8; pidx++){
		uintptr_t page_table = (uintptr_t)alloc_frame();

		unsigned int pte;
		for(pte = 0; pte < MM_PAGE_COMMON_SIZE; pte++){
			((uint32_t*)page_table)[pte] = address | 3;
			address += 0x1000;
		}

		((uintptr_t*)page_directory)[pidx] = virt_to_phys(page_table) | 3;
	}

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

	//__asm__ ("cli; hlt");
}

uintptr_t phys_to_virt(uintptr_t phys){
	return phys + KERNEL_VBASE;
}

uintptr_t virt_to_phys(uintptr_t virt){
	return virt - KERNEL_VBASE;
}
