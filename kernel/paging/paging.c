#include "paging.h"


page_directory_t *kernel_directory = 0;
page_directory_t *current_directory = 0;
uint32_t *frames;
uint32_t nframes;

extern uint32_t placement_address;

#define INDEX_FROM_BIT(a) ((a)/(8*4))
#define OFFSET_FROM_BIT(a) ((a)%(8*4))

static void set_frame(uint32_t frame_address){
  uint32_t frame = frame_address / 0x1000;
  uint32_t idx = INDEX_FROM_BIT(frame);
  uint32_t off = OFFSET_FROM_BIT(frame);
  frames[idx] |= (0x1 << off);
}

static void clear_frame(uint32_t frame_address){
  uint32_t frame = frame_address / 0x1000;
  uint32_t idx = INDEX_FROM_BIT(frame);
  uint32_t off = OFFSET_FROM_BIT(frame);
  frames[idx] &= ~(0x1 << off);
}

/*static uint32_t test_frame(uint32_t frame_address){
  uint32_t frame = frame_address / 0x1000;
  uint32_t idx = INDEX_FROM_BIT(frame);
  uint32_t off = OFFSET_FROM_BIT(frame);
  return (frames[idx] & (0x1 << off));
}*/

static uint32_t first_frame(){
  uint32_t i, j;
  for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
  {
      if (frames[i] != 0xFFFFFFFF) // nothing free, exit early.
      {
          // at least one bit is free here.
          for (j = 0; j < 32; j++)
          {
              uint32_t toTest = 0x1 << j;
              if ( !(frames[i]&toTest) )
              {
                  return i*4*8+j;
              }
          }
      }
  }
  return -5000;
}

void alloc_frame(page_t *page, uint32_t is_kernel, uint32_t is_writable){
  if(page->frame != 0){
    return;
  } else {
    uint32_t idx = first_frame();
    if(idx == (uint32_t)-1){
      panic_m("No Free Pages!");
    }
    set_frame(idx * 0x1000);
    page->present = 1;
    page->rw = (is_writable)?1:0;
    page->user = (is_kernel)?0:1;
    page->frame = idx;
  }
}

void free_frame(page_t *page){
  uint32_t frame;
  if(!(frame = page->frame)){
    return;
  } else {
    clear_frame(frame);
    page->frame = 0x0;
  }
}



void initialise_paging(){
  uint32_t mem_end_page = 0x1000000;//memSize;

  nframes = mem_end_page / 0x1000;
  frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
  memset(frames, 0, INDEX_FROM_BIT(nframes));

  kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
  memset(kernel_directory, 0, sizeof(page_directory_t));
  current_directory = kernel_directory;

  uint32_t i = 0;
  while(i < placement_address){
    page_t* usepage = get_page(i, 1, kernel_directory);
    if(usepage == 0){
      panic_m("get_page errord");
    }
    alloc_frame(usepage, 0, 0);
    i += 0x1000;
  }
  register_interrupt_handler(14, page_fault);
  switch_page_directory(kernel_directory);
}

void switch_page_directory(page_directory_t *dir){
  current_directory = dir;
  __asm__ __volatile__("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
  uint32_t cr0;
  __asm__ __volatile__("mov %%cr0, %0": "=r"(cr0));
  cr0 |= 0x80000000; // Enable paging!
  __asm__ __volatile__("mov %0, %%cr0":: "r"(cr0));
}

page_t* get_page(uint32_t address, uint32_t make, page_directory_t *dir){
  address /= 0x1000;

  uint32_t table_idx = address / 1024;
  if(dir->tables[table_idx]){
    return &dir->tables[table_idx]->pages[address%1024];
  }
  else if(make){
    uint32_t tmp;
    dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
    memset(dir->tables[table_idx], 0, 0x1000);
    dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
    return &dir->tables[table_idx]->pages[address%1024];
  } else {
    return 0;
  }
}

void page_fault(registers_t *regs){
  uint32_t faulting_address;
  __asm__ ("mov %%cr2, %0" : "=r" (faulting_address));

  int present   = !(regs->err_code & 0x1); // Page not present
  int rw = regs->err_code & 0x2;           // Write operation?
  int us = regs->err_code & 0x4;           // Processor was in user-mode?
  int reserved = regs->err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?

  // Output an error message.
  kprint("Page fault! ( ");
  if (present) {kprint("present ");}
  if (rw) {kprint("read-only ");}
  if (us) {kprint("user-mode ");}
  if (reserved) {kprint("reserved ");}
  kprint(") at ");
  char address[35] = "";
  hex_to_ascii(faulting_address, address);
  kprint(address);
  kprint("\n");
  panic_m("Page Fault");
}
