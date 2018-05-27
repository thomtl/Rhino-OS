#ifndef PAGING_H
#define PAGING_H
#include <stdint.h>
extern void loadPageDirectory(uint32_t*);
extern void enablePaging();
void starttt();

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t page_table[1024] __attribute__((aligned(4096)));

#endif
