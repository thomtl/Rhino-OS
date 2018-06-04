#include "../include/stdlib.h"
#include "../../kernel/heap/kheap.h"

void* malloc(size_t size){
  return kmalloc_a(size);
}
