#include "../include/stdlib.h"
#include <rhino/mm/hmm.h>

void* malloc(size_t size){
  return (void*)kmalloc(size);
}

void free(void* ptr){
  kfree(ptr);
}
