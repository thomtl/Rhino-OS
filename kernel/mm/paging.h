#ifndef PAGING_H
#define PAGING_H
#include <stdint.h>
#include "../../libc/include/string.h"
#include "../arch/x86/isr.h"
#include "kheap.h"
#include "../panic.h"
#include "../common.h"
#define PAGE_COMMON_SIZE 1024

#define PAGE_S 0x400000

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
	page_t pages[PAGE_COMMON_SIZE];
} page_table_t;


typedef struct {
  page_table_t* tables[PAGE_COMMON_SIZE];
} vpage_dir_t;

typedef struct{
	page_table_t* tables[1024];
	unsigned int tables_physical[1024];
	unsigned int physical_address;
} page_directory_t;

extern vpage_dir_t* root_vpage_dir;

#define EMPTY_TAB ((page_table_t*) 0x00000002)

void switch_vpage_dir(vpage_dir_t* dir);
vpage_dir_t* mk_vpage_dir();
page_table_t* mk_vpage_table();

void install_paging();

void vpage_map(vpage_dir_t* dir, uint32_t virt, uint32_t phys);


#endif
