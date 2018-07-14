#ifndef MM_PAGING_H
#define MM_PAGING_H
#include <stdint.h>
#include "../../libc/include/string.h"
#include "../arch/x86/isr.h"
#include "kheap.h"
#include "phys.h"
#include "../panic.h"
#include "../common.h"
#define MM_PAGE_COMMON_SIZE 1024

#define MM_PAGE_S 0x400000

extern unsigned int* current_dir;
extern unsigned int* root_dir;

typedef struct{
	unsigned int present	: 1;
	unsigned int rw		: 1;
	unsigned int user	: 1;
	unsigned int accessed	: 1;
	unsigned int dirty	: 1;
	unsigned int unused	: 7;
	unsigned int frame	: 20;
} page_t;

typedef struct{
	page_t pages[MM_PAGE_COMMON_SIZE];
} page_table_t;


typedef struct {
  page_table_t* tables[MM_PAGE_COMMON_SIZE];
} page_dir_t;


extern page_dir_t* root_vpage_dir;

#define MM_EMPTY_TAB ((page_table_t*) 0x00000002)

void switch_page_dir(page_dir_t* dir);
page_dir_t* mk_page_dir();
page_table_t* mk_page_table();

void init_mm_paging();

void page_map(page_dir_t* dir, uint32_t virt, uint32_t phys);


#endif
