#ifndef COMMON_H
#define COMMON_H 1
#include <stdint.h>

//#define KERNEL_VBASE 0xC0000000
#define BIT_IS_SET(var, pos) ((var) & (1 << (pos)))
#define ASSERT(b) ((b) ? (void)0 : panic_assert(__FILE__, __LINE__, #b))
void panic_assert(char *file, uint32_t line, char *desc);
#endif
