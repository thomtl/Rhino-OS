#include <rhino/mm/pmm.h>
#include <rhino/arch/x86/drivers/screen.h>
extern uint32_t _kernel_end;
extern uint32_t _kernel_start;
multiboot_info_t* mbd;
uint32_t kern_end = 0;
uint32_t kern_start = 0;
uint32_t mbd_start;
uint32_t mbd_end;

static uint32_t pmm_mem_s = 0;
static uint32_t pmm_used_blocks = 0;
static uint32_t pmm_max_blocks = 0;
static uint32_t* pmm_bitmap = 0;

void* pmm_early_kmalloc(size_t s){
    uint32_t tmp = kern_end;
    kern_end += s;
    return (void*)tmp;
}

static inline void pmm_set(uint32_t bit){
    pmm_bitmap[bit / 32] |= (1 << (bit % 32));
}

static inline void pmm_clear(uint32_t bit){
    pmm_bitmap[bit / 32] &= ~(1 << (bit % 32));
}

static inline bool pmm_test(uint32_t bit){
    return pmm_bitmap[bit / 32] & (1 << (bit % 32));
}

static uint32_t pmm_get_block_count(){
    return pmm_mem_s / 0x1000;
}

static uint32_t pmm_first_free(){
    for(uint32_t i = 0; i < pmm_get_block_count() / 32; i++){
        if(pmm_bitmap[i] != 0xffffffff){
            for(uint32_t j = 0; j < 32; j++){
                uint32_t bit = 1 << j;
                if(! (pmm_bitmap[i] & bit)){
                    return i*4*8+j;
                }
            }
        }
    }
    return -1;
}

//TODO: pmm_first_free_s(uint32_t blocks);

bool init_pmm(multiboot_info_t* mstruc){
    kern_start = (uint32_t)&_kernel_start;
    kern_end = (uint32_t)&_kernel_end;
    mbd = mstruc;
    mbd_start = (uint32_t)mbd;
    mbd_end = (uint32_t)(mbd + sizeof(multiboot_info_t));

    pmm_mem_s = (((mbd->mem_lower * 1024) + (mbd->mem_upper * 1024)) / 1024 / 1024 + 1) * 0x100000;
    pmm_used_blocks = pmm_get_block_count();
    pmm_max_blocks = pmm_get_block_count();
    pmm_bitmap = pmm_early_kmalloc((pmm_max_blocks / 32));

    memset(pmm_bitmap, 0xf, pmm_get_block_count() / PHI_PMM_BLOCKS_PER_BYTE);

    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)((uintptr_t)mbd->mmap_addr + (uintptr_t)KERNEL_VBASE);
    uintptr_t *mmap_end = (uintptr_t*)((uintptr_t)(mbd->mmap_addr + mbd->mmap_length) + (uintptr_t)KERNEL_VBASE);
    while((uint32_t)mmap < (uint32_t)mmap_end){
      if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE){
          uint32_t* addr = (uint32_t*)((uint32_t)mmap->addr);
          pmm_init_region(addr, mmap->len);
      }
      mmap = (multiboot_memory_map_t*) ( (uintptr_t)mmap + mmap->size + sizeof(uintptr_t) );
    }
    return true;
}

void pmm_init_region(uint32_t* base, size_t size){
    int align = (uint32_t)base / PHI_PMM_BLOCK_SIZE;
    int blocks = size / PHI_PMM_BLOCK_SIZE;
    for(; blocks > 0; blocks--){
        pmm_clear(align++);
        pmm_used_blocks--;
    }

    pmm_set(0);
}

void pmm_deinit_region(uint32_t* base, size_t size){
    int align = (uint32_t)base / PHI_PMM_BLOCK_SIZE;
    int blocks = size /PHI_PMM_BLOCK_SIZE;

    for(; blocks > 0; blocks--){
        pmm_set(align++);
        pmm_used_blocks++;
    }
}

void* pmm_alloc_block(){
    uint32_t frame;
    if((frame = pmm_first_free()) == (uint32_t)-1){
        kprint("[PMM]: No free Blocks left\n");
        return 0;
    }
    uint32_t addr = frame * PHI_PMM_BLOCK_SIZE;
    if((addr >= (kern_start - (uint32_t)KERNEL_VBASE) && addr <= (kern_end - (uint32_t)KERNEL_VBASE)) || (addr >= (mbd_start - (uint32_t)KERNEL_VBASE) && addr <= (mbd_end - (uint32_t)KERNEL_VBASE))){
        pmm_set(frame);
        pmm_used_blocks++;
        return pmm_alloc_block();
    }
    pmm_set(frame);

    pmm_used_blocks++;
    return (void*)addr;

}
void pmm_free_block(void* block){
    uint32_t addr = (uint32_t)block;
    int frame = addr / PHI_PMM_BLOCK_SIZE;

    pmm_clear(frame);

    pmm_used_blocks--;
}
