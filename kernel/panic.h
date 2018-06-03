#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H
#include "../drivers/screen.h"
void panic();
void panic_m(char* m);
#endif
