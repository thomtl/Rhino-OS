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

char* dtoa(double x, char* p){
    uint64_t n;
    uint32_t i = 0, k = 0;
    n = (uint64_t)x;
    while(n>0){
        x /= 10;
        n = (uint64_t)x;
        i++;
    }
    *(p+1) = '.';
    x *= 10;
    n = (uint64_t)x;
    x = x - n;
    while((n>0)||(i>k)){
        if(k == i) k++;
        *(p+k) = '0'+n;
        x *= 10;
        n = (uint64_t)x;
        x = x - n;
        k++;
    }
    *(p+k) = '\0';
    return p;
}