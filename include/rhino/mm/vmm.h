#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <rhino/mm/pmm.h>
#include <rhino/common.h>

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

#define RHINO_VMM_PAGES_PER_TABLE 1024
#define RHINO_VMM_PAGES_PER_DIR 1024
#define RHINO_VMM_PTABLE_ADDR_SPACE_SIZE 0x400000
#define RHINO_VMM_DTABLE_ADDR_SPACE_SIZE 0x100000000

#define RHINO_VMM_PAGE_SIZE 4096

typedef enum {
    RHINO_PTE_PRESENT = 1,
    RHINO_PTE_WRITABLE = 2,
    RHINO_PTE_USER = 4,
    RHINO_PTE_WRITETHROUGH = 8,
    RHINO_PTE_NOT_CACHEABLE = 0x10,
    RHINO_PTE_ACCESSED = 0x20,
    RHINO_PTE_DIRTY = 0x40,
    RHINO_PTE_PAT = 0x80,
    RHINO_PTE_CPU_GLOBAL = 0x100,
    RHINO_PTE_LV4_GLOBAL = 0x200,
    RHINO_PTE_FRAME = 0x7FFFF000
} RHINO_PTE_FLAGS_T;

typedef enum {
    RHINO_PDE_PRESENT = 1,
    RHINO_PDE_WRITABLE = 2,
    RHINO_PDE_USER = 4,
    RHINO_PDE_PWT = 8,
    RHINO_PDE_PCD = 0x10,
    RHINO_PDE_ACCESSED = 0x20,
    RHINO_PDE_DIRTY = 0x40,
    RHINO_PDE_4MB = 0x80,
    RHINO_PDE_CPU_GLOBAL = 0x100,
    RHINO_PDE_LV4_GLOBAL = 0x200,
    RHINO_PDE_FRAME = 0x7FFFF000
} RHINO_PDE_FLAGS_T;

typedef uint32_t pt_entry;
typedef uint32_t pd_entry;

typedef struct {
    pt_entry m_entries[RHINO_VMM_PAGES_PER_TABLE];
} ptable;

typedef struct {
    pd_entry m_entries[RHINO_VMM_PAGES_PER_DIR];
} pdirectory;

extern void tlb_flush();

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
void vmm_free_dir(pdirectory* dir);

void vmm_flush_tlb_entry(void* addr);
void vmm_map_page(void* phys, void* virt, uint32_t user);
void vmm_unmap_page(void* virt);
bool vmm_page_is_mapped(void* virt);

void vmm_ptable_clear(ptable* tab);
void vmm_pdirectory_clear(pdirectory* dir);

bool init_vmm();

void* vmm_virt_to_phys(void* virt);
