#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H
#include "../drivers/screen.h"
__attribute__((noreturn))
void panic();
__attribute__((noreturn))
void panic_m(char* m);
#endif
