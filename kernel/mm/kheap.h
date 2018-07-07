#ifndef KMALLOC_H
#define KMALLOC_H
#include <stdint.h>
#include <stddef.h>
#include "../common.h"
extern uint32_t end;
#define KHEAP_MAGIC 0x04206969
#define KHEAP_MAGIC2 0xCAFEC0DE

#define KHEAP_END 0xFFFFDEAD

#define MEM_END 0x8000000

typedef struct {
  uint32_t magic;
  bool free;
  uint32_t size;
  uint32_t magic2;
} heap_header_t;

typedef struct {
  uint32_t magic;
  uint32_t size;
  uint32_t magic2;
} heap_footer_t;

#define HEAP_S (sizeof(heap_header_t))
#define HEAP_TOTAL (sizeof(heap_footer_t) + HEAP_S)
#define HEAP_MINIMUM 1
#define HEAP_FIND_SIZE (HEAP_TOTAL + HEAP_MINIMUM)
#define KHEAP_SIZE 0xFFFFF
void init_heap();


void kfree(void *p);
void* kmalloc_int(size_t sz, int align, uint32_t *phys);

void* kmalloc_a(size_t sz);

void* kmalloc_p(size_t sz, uint32_t *phys);

void* kmalloc_ap(size_t sz, uint32_t *phys);

void* kmalloc(size_t sz);
#endif
