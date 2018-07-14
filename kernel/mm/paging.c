#include "paging.h"

extern uint32_t placement_address;

extern void tlb_flush();


page_dir_t* current_page_dir = NULL;
page_dir_t* root_page_dir = NULL;


/*static page_table_t* read_cr3(){
	unsigned int cr3;

	__asm__ __volatile__ ("movl %%cr3, %%eax" : "=a" (cr3));
	return (page_table_t*) cr3;
}*/

static unsigned int read_cr0(){
	unsigned int cr0;

	__asm__ __volatile__ ("movl %%cr0, %%eax" : "=a" (cr0));
	return cr0;
}

static void write_cr3(page_dir_t* dir){
	unsigned int addr = (unsigned int) &dir->tables[0];
	__asm__ __volatile__ ("movl %%eax, %%cr3" :: "a" (addr));
}

static void write_cr0(unsigned int new_cr0){
	__asm__ __volatile__ ("movl %%eax, %%cr0" :: "a" (new_cr0));
}





void switch_page_dir(page_dir_t* dir){
  write_cr3(dir);
  write_cr0(read_cr0() | 0x80000001);
}

page_dir_t* mk_page_dir(){
  page_dir_t* dir = (page_dir_t*) alloc_frame();

  for(uint32_t i = 0; i < MM_PAGE_COMMON_SIZE; i++){
    dir->tables[i] = MM_EMPTY_TAB;
  }

  return dir;
}
page_table_t* mk_page_table(){
  page_table_t* tab = (page_table_t*) alloc_frame();

  for(uint32_t i = 0; i < MM_PAGE_COMMON_SIZE; i++){
    tab->pages[i].present = 0;
    tab->pages[i].rw = 0;
  }
  return tab;
}
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
  register_interrupt_handler(14, page_fault);

  current_page_dir = mk_page_dir();
  root_page_dir = current_page_dir;

  for(uint32_t i = 0; i < placement_address; i += MM_PAGE_S){
    page_map(root_page_dir, i, i);
  }

  CLI();
  switch_page_dir(root_page_dir);
  STI();
}

void page_map(page_dir_t* dir, uint32_t virt, uint32_t phys){
  uint16_t id = virt >> 22;
  page_table_t* tab = mk_page_table();
  dir->tables[id] = ((page_table_t*)((unsigned int) tab | 3 | 4));

  for(uint32_t i = 0; i < MM_PAGE_COMMON_SIZE; i++){
    tab->pages[i].frame = phys >> 12;
    tab->pages[i].present = 1;
    phys += 4096;
  }
  tlb_flush();
}
