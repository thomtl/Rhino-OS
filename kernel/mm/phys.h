#ifndef MM_PHYS_H
#define MM_PHYS_H
#include <stdint.h>
#include "../multiboot.h"

void init_mm_phys_manager(multiboot_info_t *mbd);
void* alloc_frame();
void free_frame(void* frame);
#endif
