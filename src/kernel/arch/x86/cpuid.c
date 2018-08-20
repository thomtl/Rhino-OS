#include <rhino/arch/x86/cpuid.h>

inline void cpuid(uint32_t code, uint32_t *a, uint32_t *d){
    asm("cpuid" : "=a"(*a), "=d"(*d) : "a"(code) : "ecx", "ebx");
}