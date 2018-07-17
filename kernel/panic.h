#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H
#include <stdint.h>
#include "../drivers/screen.h"
#include "../libk/include/string.h"
__attribute__((noreturn))
void panic();

__attribute__((noreturn))
void panic_m(char* m, char *f);

__attribute__((noreturn))
void panic_assert(char *file, uint32_t line, char *desc);
#endif
