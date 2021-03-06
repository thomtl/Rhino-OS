#ifndef RHINO_LIBC_STDLIB_H
#define RHINO_LIBC_STDLIB_H
#include "cdefs.h"
#include <stdint.h>
#include <stddef.h>
#include "sys.h"

void* malloc(size_t sz);
void free(void* addr);

void exit();
char* dtoa(double x, char* p);

char* itoa(int32_t value, char* str, uint32_t base);

#endif
