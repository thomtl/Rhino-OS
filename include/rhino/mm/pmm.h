#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <rhino/multiboot.h>
#include <libk/string.h>

#define RHINO_PMM_BLOCKS_PER_BYTE 8
#define RHINO_PMM_BLOCK_SIZE 4096
#define RHINO_PMM_BLOCK_ALIGN RHINO_PMM_BLOCK_SIZE


bool init_pmm(multiboot_info_t* mbd);
void* pmm_early_kmalloc(size_t s);
void pmm_init_region(uint32_t* base, size_t size);
void pmm_deinit_region(uint32_t* base, size_t size);
void* pmm_alloc_block();
void pmm_free_block(void* block);