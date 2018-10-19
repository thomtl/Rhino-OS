#include <rhino/arch/x86/msr.h>

uint32_t msr_check(){
    uint32_t a, c, d;
    cpuid(CPUID_GET_FEATURES, &a, &c, &d);
    return BIT_IS_SET(d, CPUID_FEAT_EDX_MSR);
}

void msr_read(uint32_t msr, uint32_t *lo, uint32_t *hi){
    asm("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}
void msr_write(uint32_t msr, uint32_t lo, uint32_t hi){
    asm("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}