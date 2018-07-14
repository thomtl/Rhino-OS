#ifndef MM_PHYS_H
#define MM_PHYS_H
#include <stdint.h>
#include "../multiboot.h"

#define MMAP_GET_NUM 0
#define MMAP_GET_ADDR 1
#define PAGE_SIZE 4096


void init_phys_manager(multiboot_info_t *mbd);
void* alloc_frame();
void free_frame(uint32_t* frame);
#endif
