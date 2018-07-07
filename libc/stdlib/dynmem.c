#include "../include/stdlib.h"
#include "../../kernel/mm/kheap.h"

void* malloc(size_t size){
  return (void*)kmalloc(size);
}

void free(void* ptr){
  kfree(ptr);
}

void* realloc(void *ptr, size_t size){
  return krealloc(ptr, size);
}
