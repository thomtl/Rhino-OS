#include "include/stdlib.h"

void* malloc(size_t sz){
    return (void*)syscall(2, 1, sz, 0);
}
void free(void* addr){
    syscall(2, 2, (uint32_t)addr, 0);
}

void exit(){
    syscall(0, 5, 0, 0);
}