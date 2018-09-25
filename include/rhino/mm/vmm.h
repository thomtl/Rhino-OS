#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <rhino/mm/pmm.h>
#include <rhino/common.h>

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

#define PHI_VMM_PAGES_PER_TABLE 1024
#define PHI_VMM_PAGES_PER_DIR 1024
#define PHI_VMM_PTABLE_ADDR_SPACE_SIZE 0x400000
#define PHI_VMM_DTABLE_ADDR_SPACE_SIZE 0x100000000

#define PHI_VMM_PAGE_SIZE 4096

typedef enum {
    PHI_PTE_PRESENT = 1,
    PHI_PTE_WRITABLE = 2,
    PHI_PTE_USER = 4,
    PHI_PTE_WRITETHROUGH = 8,
    PHI_PTE_NOT_CACHEABLE = 0x10,
    PHI_PTE_ACCESSED = 0x20,
    PHI_PTE_DIRTY = 0x40,
    PHI_PTE_PAT = 0x80,
    PHI_PTE_CPU_GLOBAL = 0x100,
    PHI_PTE_LV4_GLOBAL = 0x200,
    PHI_PTE_FRAME = 0x7FFFF000
} PHI_PTE_FLAGS_T;

typedef enum {
    PHI_PDE_PRESENT = 1,
    PHI_PDE_WRITABLE = 2,
    PHI_PDE_USER = 4,
    PHI_PDE_PWT = 8,
    PHI_PDE_PCD = 0x10,
    PHI_PDE_ACCESSED = 0x20,
    PHI_PDE_DIRTY = 0x40,
    PHI_PDE_4MB = 0x80,
    PHI_PDE_CPU_GLOBAL = 0x100,
    PHI_PDE_LV4_GLOBAL = 0x200,
    PHI_PDE_FRAME = 0x7FFFF000
} PHI_PDE_FLAGS_T;

typedef uint32_t pt_entry;
typedef uint32_t pd_entry;

typedef struct {
    pt_entry m_entries[PHI_VMM_PAGES_PER_TABLE];
} ptable;

typedef struct {
    pd_entry m_entries[PHI_VMM_PAGES_PER_DIR];
} pdirectory;

void vmm_pt_entry_add_attrib(pt_entry* e, uint32_t attrib);
void vmm_pt_entry_del_attrib(pt_entry* e, uint32_t attrib);
void vmm_pt_entry_set_frame(pt_entry* e, void* frame);
bool vmm_pt_entry_is_present(pt_entry e);
bool vmm_pt_entry_is_writable(pt_entry e);
void* vmm_pt_entry_pfn(pt_entry e);

void vmm_pd_entry_add_attrib(pd_entry* e, uint32_t attrib);
void vmm_pd_entry_del_attrib(pd_entry* e, uint32_t attrib);
void vmm_pd_entry_set_frame(pd_entry* e, void* frame);
bool vmm_pd_entry_is_present(pd_entry e);
bool vmm_pd_entry_is_user(pd_entry e);
bool vmm_pd_entry_is_writable(pd_entry e);
bool vmm_pd_entry_is_4mb(pd_entry e);
void* vmm_pd_entry_pfn(pd_entry e);
void vmm_pd_entry_enable_global(pd_entry e);

bool vmm_alloc_page(pt_entry* e);
void vmm_free_page(pt_entry* e);

pt_entry* vmm_ptable_lookup_entry(ptable* p, void* addr);
pd_entry* vmm_pdirectory_lookup_entry(pdirectory* p, void* addr);

bool vmm_switch_pdirectory(pdirectory* dir);
pdirectory* vmm_get_directory();

pdirectory* vmm_clone_dir(pdirectory* dir);

void vmm_flush_tlb_entry(void* addr);
void vmm_map_page(void* phys, void* virt, uint32_t user);

void vmm_ptable_clear(ptable* tab);
void vmm_pdirectory_clear(pdirectory* dir);

bool init_vmm();
