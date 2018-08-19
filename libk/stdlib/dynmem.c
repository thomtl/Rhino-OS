#include "../include/stdlib.h"
#include "../../kernel/mm/hmm.h"

void* malloc(size_t size){
  return (void*)kmalloc(size);
}

void free(void* ptr){
  kfree(ptr);
}
