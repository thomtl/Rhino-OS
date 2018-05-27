#include "../include/stdlib.h"
#include "../../kernel/memory/kmalloc.h"

void* malloc(size_t size){
  return kmalloc(size);
}
