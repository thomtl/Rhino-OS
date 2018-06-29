#include "msr.h"

uint32_t msr_check(){
    uint32_t a, d;
    cpuid(1, &a, &d);
    return d & CPUID_MSR_FLAG;
}

void msr_read(uint32_t msr, uint32_t *lo, uint32_t *hi){
    __asm__ __volatile__ ("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}
void msr_write(uint32_t msr, uint32_t lo, uint32_t hi){
    __asm__ __volatile__ ("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}