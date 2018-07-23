#ifndef MM_PAGING_H
#define MM_PAGING_H
#include <stdint.h>
#include "../../libk/include/string.h"
#include "../arch/x86/isr.h"
#include "kheap.h"
#include "phys.h"
#include "../panic.h"
#include "../common.h"
#define MM_PAGE_COMMON_SIZE 1024

#define MM_PAGE_S 0x400000


void init_mm_paging();

void map_phys_virt(uintptr_t page_directory, uintptr_t phys, uintptr_t virt);
void unmap_phys_virt(uintptr_t page_directory, uintptr_t virt);

uintptr_t phys_to_virt(uintptr_t phys);
uintptr_t virt_to_phys(uintptr_t virt);

// TODO: Clone page_dirs and give tasks own dir

#endif
