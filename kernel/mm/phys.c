#include "phys.h"
#include "kheap.h"
#include "../../drivers/screen.h"
#include "../common.h"
multiboot_info_t *mboot_hdr;
uint32_t mboot_reserved_start;
uint32_t mboot_reserved_end;

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

uint32_t kernel_end = (uint32_t)&_kernel_end;
uint32_t kernel_start = (uint32_t)&_kernel_start;

uint32_t nframes;
uint32_t* frames;

#define INDEX_FROM_BIT(a) ((a) / (8 * 4))
#define OFFSET_FROM_BIT(a) ((a) % (8 * 4))
#define IDX_TO_ADDR(a) ((a) * (0x1000))
#define ADDR_TO_IDX(a) ((a) / (0x1000))


/**
   @brief Sets a bit in the bitmap for a frame.
   @param args List of args.  args[0] is the address of the frame to set.
 */
static void set_frame(uint32_t addr){
    uint32_t frame = ADDR_TO_IDX(addr);
    uint32_t i = INDEX_FROM_BIT(frame);
    uint32_t offset = OFFSET_FROM_BIT(frame);
    frames[i] |= (0x1 << offset);
}


/**
   @brief Clears a bit in the bitmap for a frame.
   @param args List of args.  args[0] is the address of the frame to set.
 */
static void clear_frame(uint32_t addr){
    uint32_t frame = ADDR_TO_IDX(addr);
    uint32_t i = INDEX_FROM_BIT(frame);
    uint32_t offset = OFFSET_FROM_BIT(frame);
    frames[i] &= ~(0x1 << offset);
}

/**
   @brief Checks if the bit in the bitmap is set for a frame.
   @param args List of args.  args[0] is the address of the frame to check.
   @return returns a bool for the state of the bit.
 */
static bool test_frame(uint32_t addr){
  uint32_t frame = ADDR_TO_IDX(addr);
  uint32_t i = INDEX_FROM_BIT(frame);
  uint32_t offset = OFFSET_FROM_BIT(frame);
  return frames[i] & (0x1 << offset);
}


/**
   @brief Looks in the bitmap and finds the first free bit.
   @return returns the index in the bitmap for the first free frame.
 */
static uint32_t first_frame(){
    uint32_t i, j;
    for(i = 0; i < INDEX_FROM_BIT(nframes); i++){
        if(frames[i] != 0xFFFFFFFF){
            for(j = 0; j < 32; j++){
                uint32_t toTest = 0x1 << j;
                if(!(frames[i]&toTest)){
                    return i*4*8+j;
                }
            }
        }
    }
    return 0xDEADBEEF;
}


/**
   @brief Initialize the physical memory manager.
   @param args List of args.  args[0] is the pointer to the multiboot info structure.
 */
void init_mm_phys_manager(multiboot_info_t *mbd){
    mboot_hdr = mbd;
    mboot_reserved_start = (uint32_t)mboot_hdr;
    mboot_reserved_end = (uint32_t)(mboot_hdr + sizeof(multiboot_info_t));

    uint32_t mem = (((mbd->mem_lower * 1024) + (mbd->mem_upper * 1024)) / 1024 / 1024 + 1) * 0x100000;
    nframes = mem / 0x1000;
    frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));
}


/**
   @brief Reads the memory map for a frame and checks if it is available.
   @param args List of args.  args[0] is the frame to check for.
   @return returns a bool for if it is reserved.
 */
bool read_mmap(uint32_t addr){
  multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mboot_hdr->mmap_addr;
  uintptr_t *mmap_end = (uintptr_t*)(mboot_hdr->mmap_addr + mboot_hdr->mmap_length);
  while((uint32_t)mmap < (uint32_t)mmap_end){
    if((addr >= mmap->addr) && (addr <= (mmap->addr + mmap->len))){
      if((mmap->type == MULTIBOOT_MEMORY_RESERVED) || (mmap->type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) || (mmap->type == MULTIBOOT_MEMORY_NVS) || (mmap->type == MULTIBOOT_MEMORY_BADRAM)){
        return true;
      } else if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE){
        return false;
      }
    }
    mmap = (multiboot_memory_map_t*) ( (uintptr_t)mmap + mmap->size + sizeof(uintptr_t) );
  }
  return true;
}


/**
   @brief Allocates a physical frame.
   @return returns a pointer to the allocated frame.
 */
void* alloc_frame(){
    uint32_t idx = first_frame();
    if(idx == 0xDEADBEEF) PANIC_M("No free physical frames left");

    bool reserved = read_mmap(IDX_TO_ADDR(idx));
    if(reserved){
      set_frame(IDX_TO_ADDR(idx));
      return alloc_frame();
    }
    uint32_t addr = IDX_TO_ADDR(idx);
    if((addr >= mboot_reserved_start && addr <= mboot_reserved_end ) || (addr >= kernel_start && addr <= kernel_end)){
        set_frame(IDX_TO_ADDR(idx));
        return alloc_frame();
    }

    set_frame(IDX_TO_ADDR(idx));
    return (void*)(IDX_TO_ADDR(idx));
}

/**
   @brief Frees an allocated frame.
   @param args List of args.  args[0] is the address of the frame to free.
 */
void free_frame(void* frame){
  if(test_frame((uint32_t)frame)) clear_frame((uint32_t)frame * 0x1000);
}
