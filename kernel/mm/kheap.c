#include "kheap.h"
#include "../../libc/include/string.h"
#include "../../drivers/screen.h"
#include "paging.h"
#include "../common.h"

uint32_t placement_address = (uint32_t)&end;
heap_header_t* kheap = NULL;

static void start_heap(heap_header_t *heap, size_t size){
    heap->magic = KHEAP_MAGIC;
    heap->magic2 = KHEAP_MAGIC2;
    heap->free = true;
    heap->size = size - HEAP_TOTAL;

    heap_footer_t* foot = (heap_footer_t*)((uint32_t)heap + HEAP_S + heap->size);
    foot->magic = KHEAP_MAGIC;
    foot->magic2 = KHEAP_MAGIC2;
    foot->size = KHEAP_END;
}

static heap_header_t* find_sized_heap(heap_header_t* heap, size_t size){
    while((heap->size < HEAP_FIND_SIZE + size) || (heap->free != true)){
        ASSERT(heap->magic == KHEAP_MAGIC);
        ASSERT(heap->magic2 == KHEAP_MAGIC2);
        heap_footer_t *foot = (heap_footer_t*) ((uint32_t) heap + HEAP_S + heap->size);
        ASSERT(foot->magic == KHEAP_MAGIC);
        ASSERT(foot->magic2 == KHEAP_MAGIC2);

        if(foot->size == KHEAP_END){
            PANIC_M("heap out of space");
        }

        if(foot->size != heap->size){
            PANIC_M("heap footer/header size mismatch");
        }

        heap = (heap_header_t*) ((uint32_t) foot + sizeof(heap_footer_t));

    }
    return heap;
}

static void split_heap(heap_header_t* heap, size_t size){
  heap_footer_t* foot = (heap_footer_t*) ((uint32_t) heap + HEAP_S + size);
  foot->magic = KHEAP_MAGIC;
	foot->magic2 = KHEAP_MAGIC2;
	foot->size = size;

	size_t new_size = heap->size - HEAP_TOTAL - size;
	heap->size = size;

	heap = (heap_header_t*) ((uint32_t) foot + sizeof(heap_footer_t));
	heap->size = new_size;
	heap->free = true;
	heap->magic = KHEAP_MAGIC;
  heap->magic2 = KHEAP_MAGIC2;

  foot = (heap_footer_t*)((uint32_t) heap + HEAP_S + heap->size);
  if((foot->magic != KHEAP_MAGIC) || (foot->magic2 != KHEAP_MAGIC2)){
	   kprint_warn("invalid footer in split\n");
	}
  if(foot->size != KHEAP_END) foot->size = new_size;
}
// leave of at free_internal

static void free_internal(heap_header_t* heap, void* address){
    heap_header_t *head = (heap_header_t*)((uint32_t) address - HEAP_S);
    if(head == heap){
        kprint_warn("kheap: cannot collapse top of heap");
        head->free = true;
        return;
    }

    if((head->magic != KHEAP_MAGIC) || (head->magic2 != KHEAP_MAGIC2)){
        kprint_warn("kheap: invalid magic1");
        return;
    }

    heap_footer_t *foot = (heap_footer_t*)((uint32_t) head + HEAP_S + head->size);
    if((foot->magic != KHEAP_MAGIC) || (foot->magic2 != KHEAP_MAGIC2)){
        kprint_warn("kheap: invalid magic2");
        return;
    }

    foot = (heap_footer_t*)((uint32_t) head - sizeof(heap_footer_t));
    if((foot->magic != KHEAP_MAGIC) || (foot->magic2 != KHEAP_MAGIC2)){
        kprint_warn("kheap: invalid magic3");
        return;
    }

    if(foot->size == KHEAP_END) PANIC_M("impossible condition for heap");

    heap = (heap_header_t*)((uint32_t)foot - foot->size - HEAP_S);
    if((heap->magic != KHEAP_MAGIC) || (heap->magic2 != KHEAP_MAGIC2)){
        kprint_warn("kheap: invalid magic4");
        return;
    }

    foot = (heap_footer_t*)((uint32_t)heap + (heap->size + head->size + HEAP_TOTAL) + HEAP_S);
    if((foot->magic != KHEAP_MAGIC) || (foot->magic2 != KHEAP_MAGIC2)){
        kprint_warn("kheap: invalid magic5");
        return;
    }

    heap->size += head->size + HEAP_TOTAL;
    foot->size = heap->size;
}

void* alloc_internal(heap_header_t* heap, size_t size){
    heap = find_sized_heap(heap, size + 8);
    heap->free = false;
    split_heap(heap, size);
    return (void*)((uint32_t) heap + HEAP_S);
}



void init_heap(){
    kheap = (heap_header_t*) kmalloc(KHEAP_SIZE);
    start_heap(kheap, KHEAP_SIZE);

}

void* kmalloc_int(size_t sz, int align, uint32_t *phys)
{
    if(kheap != 0){
      void *addr = alloc_internal(kheap, sz);
      return (void*)addr;
    } else {
      if (align == 1 && (placement_address & 0xFFFFF000) )
      {
          // Align the placement address;
          placement_address &= 0xFFFFF000;
          placement_address += 0x1000;
      }
      if (phys)
      {
          *phys = placement_address;
      }
      uint32_t tmp = placement_address;
      placement_address += sz;
      return (void*)(tmp + KERNEL_VBASE);
  }
}
void kfree(void *p)
{
    free_internal(kheap, p);
    return;
}


void* kmalloc_a(size_t sz)
{
    return kmalloc_int(sz, 1, 0);
}

void* kmalloc_p(size_t sz, uint32_t *phys)
{
    return kmalloc_int(sz, 0, phys);
}

void* kmalloc_ap(size_t sz, uint32_t *phys)
{
    return kmalloc_int(sz, 1, phys);
}

void* kmalloc(size_t sz)
{
    return kmalloc_int(sz, 0, 0);
}
