#include "paging.h"

extern uint32_t placement_address;

extern void tlb_flush();


vpage_dir_t* current_vpage_dir = NULL;
vpage_dir_t* root_vpage_dir = NULL;


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

static void write_cr3(vpage_dir_t* dir){
	unsigned int addr = (unsigned int) &dir->tables[0];
	__asm__ __volatile__ ("movl %%eax, %%cr3" :: "a" (addr));
}

static void write_cr0(unsigned int new_cr0){
	__asm__ __volatile__ ("movl %%eax, %%cr0" :: "a" (new_cr0));
}





void switch_vpage_dir(vpage_dir_t* dir){
  write_cr3(dir);
  write_cr0(read_cr0() | 0x80000001);
}

vpage_dir_t* mk_vpage_dir(){
  vpage_dir_t* dir = (vpage_dir_t*) kmalloc_a(sizeof(vpage_dir_t));

  for(uint32_t i = 0; i < PAGE_COMMON_SIZE; i++){
    dir->tables[i] = EMPTY_TAB;
  }

  return dir;
}
page_table_t* mk_vpage_table(){
  page_table_t* tab = (page_table_t*) kmalloc_a(sizeof(page_table_t));

  for(uint32_t i = 0; i < PAGE_COMMON_SIZE; i++){
    tab->pages[i].present = 0;
    tab->pages[i].rw = 0;
  }
  tlb_flush();
  return tab;
}
void vpage_fault(registers_t* regs){
	unsigned int err_pos;
	__asm__ __volatile__ ("mov %%cr2, %0" : "=r" (err_pos));

	kprint("Page fault occurred at ");
  char address[35] = "";
  hex_to_ascii(err_pos, address);
  kprint(address);

	kprint("\nReasons:");

	int no_page = regs->err_code & 1;
	int rw = regs->err_code & 2;
	int um = regs->err_code & 4;
	int re = regs->err_code & 8;
	int dc = regs->err_code & 16;

	if(dc)		kprint(" (Instruction decode error) ");
	if(!no_page)	kprint(" (No page present) ");
	if(um)		kprint(" (in user mode) ");
	if(rw)		kprint(" (Write permissions) ");
	if(re)		kprint(" (RE) ");

	kprint("\n");
  __asm__ __volatile__ ("cli");
  while(1);
}

void install_paging(){
  register_interrupt_handler(14, vpage_fault);

  current_vpage_dir = mk_vpage_dir();
  root_vpage_dir = current_vpage_dir;

  for(uint32_t i = 0; i < placement_address; i += PAGE_S){
    vpage_map(root_vpage_dir, i, i);
  }

  __asm__ __volatile__ ("cli");
  switch_vpage_dir(root_vpage_dir);
  __asm__ __volatile__ ("sti");
}

void vpage_map(vpage_dir_t* dir, uint32_t virt, uint32_t phys){
  uint16_t id = virt >> 22;
  page_table_t* tab = mk_vpage_table();
  dir->tables[id] = ((page_table_t*)((unsigned int) tab | 3 | 4));

  for(uint32_t i = 0; i < PAGE_COMMON_SIZE; i++){
    tab->pages[i].frame = phys >> 12;
    tab->pages[i].present = 1;
    phys += 4096;
  }
}
