#ifndef MM_PHYS_H
#define MM_PHYS_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../multiboot.h"
#include "../../libk/include/string.h"

#define PHI_PMM_BLOCKS_PER_BYTE 8
#define PHI_PMM_BLOCK_SIZE 4096
#define PHI_PMM_BLOCK_ALIGN PHI_PMM_BLOCK_SIZE


bool init_pmm(multiboot_info_t* mbd);
void* pmm_early_kmalloc(size_t s);
void pmm_init_region(uint32_t* base, size_t size);
void pmm_deinit_region(uint32_t* base, size_t size);
void* pmm_alloc_block();
void pmm_free_block(void* block);



#endif
