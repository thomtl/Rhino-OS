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

static void set_frame(uint32_t addr){
    uint32_t frame = ADDR_TO_IDX(addr);
    uint32_t i = INDEX_FROM_BIT(frame);
    uint32_t offset = OFFSET_FROM_BIT(frame);
    frames[i] |= (0x1 << offset);
}

static void clear_frame(uint32_t addr){
    uint32_t frame = ADDR_TO_IDX(addr);
    uint32_t i = INDEX_FROM_BIT(frame);
    uint32_t offset = OFFSET_FROM_BIT(frame);
    frames[i] &= ~(0x1 << offset);
}

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
    //return -1;
}

void init_phys_manager(multiboot_info_t *mbd){
    mboot_hdr = mbd;
    mboot_reserved_start = (uint32_t)mboot_hdr;
    mboot_reserved_end = (uint32_t)(mboot_hdr + sizeof(multiboot_info_t));

    uint32_t mem = (((mbd->mem_lower * 1024) + (mbd->mem_upper * 1024)) / 1024 / 1024 + 1) * 0x100000;
    nframes = mem / 0x1000;
    frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));
}

uint32_t mmap_read(uint32_t request, uint8_t mode){
    if(request == 0) return 0;
    if(mode != MMAP_GET_NUM && mode != MMAP_GET_ADDR) return 0;

    uintptr_t cur_mmap_addr = (uintptr_t)mboot_hdr->mmap_addr;
    uintptr_t mmap_end_addr = cur_mmap_addr + mboot_hdr->mmap_length;
    uint32_t cur_num = 0;
    while(cur_mmap_addr < mmap_end_addr){
        multiboot_memory_map_t *current_entry = (multiboot_memory_map_t*)cur_mmap_addr;
        uint64_t i;
        uint64_t current_entry_end = current_entry->addr + current_entry->len;
        for(i = current_entry->addr; i + PAGE_SIZE < current_entry_end; i += PAGE_SIZE){
            if(mode == MMAP_GET_NUM && request >= i && request <= i + PAGE_SIZE){
                return cur_num + 1;
            }
            if(current_entry->type == MULTIBOOT_MEMORY_RESERVED){
                if(mode == MMAP_GET_ADDR && cur_num == request){
                    ++request;
                }
                ++cur_num;
                continue;
            } else if(mode == MMAP_GET_ADDR && cur_num == request){
                return i;
            }
            ++cur_num;
        }

        cur_mmap_addr += current_entry->size + sizeof(uintptr_t);

    }
    return 0;
}

void* alloc_frame(){
    uint32_t idx = first_frame();
    if(idx == (uint32_t)-1) PANIC_M("No free physical frames left");
    //uint32_t addr = mmap_read(idx, MMAP_GET_ADDR);
    //frame_t frame;
    uint32_t addr = IDX_TO_ADDR(idx);
    if((addr >= mboot_reserved_start && addr <= mboot_reserved_end ) || (addr >= kernel_start && addr <= kernel_end)){
        set_frame(idx * 0x1000);
        return alloc_frame();
    }
    ///set_frame(idx * 1000);
    //frame.num = idx;//mmap_read(addr, MMAP_GET_NUM);
    //frame.addr = idx * 0x1000;//mmap_read(frame.num, MMAP_GET_ADDR);
    set_frame(IDX_TO_ADDR(idx));
    return (void*)(IDX_TO_ADDR(idx));
}

void free_frame(uint32_t* frame){
  clear_frame((uint32_t)frame * 0x1000);
}
