#include <rhino/common.h>

uint32_t save_eflags()
{
    uint32_t eflags;
    asm("pushf; pop %0"
        : "=r"(eflags));
    return eflags;
}

void load_eflags(uint32_t eflags)
{
    asm("push %0; popf" ::"r"(eflags));
}