#include "kheap.h"
#include "../../libc/include/string.h"

#define MEMORY_ADDRESS 0x7ffff
uint32_t free_mem = MEMORY_ADDRESS;
void* kmalloc_a(size_t size){
  if(free_mem & 0xFFFFF000){
    free_mem &= 0xFFFFF000;
    free_mem += 0x1000;
  }
  uint32_t* tmp = (uint32_t*)free_mem;
  free_mem += size;
  return tmp;
}
void* kmalloc(size_t size){
  uint32_t* tmp = (uint32_t*)free_mem;
  free_mem += size;
  return tmp;
}
void* kmalloc_ap(size_t size, uint32_t* phys){
  if(free_mem & 0xFFFFF000){
    free_mem &= 0xFFFFF000;
    free_mem += 0x1000;
  }
  if(phys){
    *phys = free_mem;
  }
  uint32_t* tmp = (uint32_t*)free_mem;
  free_mem += size;
  return tmp;
}
void* kmalloc_p(size_t size, uint32_t* phys){
  if(phys){
    *phys = free_mem;
  }
  uint32_t* tmp = (uint32_t*)free_mem;
  free_mem += size;
  return tmp;
}
