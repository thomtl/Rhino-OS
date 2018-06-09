#include "common.h"
#include "../drivers/screen.h"
#include "../libc/include/string.h"
void panic_assert(char *file, uint32_t line, char *desc)
{
    // An assertion failed, and we have to panic.
    __asm__ __volatile__("cli"); // Disable interrupts.

    kprint("ASSERTION-FAILED(");
    kprint(desc);
    kprint(") at ");
    kprint(file);
    kprint(":");
    char c[5];
    int_to_ascii(line, c);
    kprint(c);
    kprint("\n");
    // Halt by going into an infinite loop.
    for(;;);
}
