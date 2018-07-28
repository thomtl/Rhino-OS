#ifndef RHINO_LIBC_STDLIB_H
#define RHINO_LIBC_STDLIB_H
#include "cdefs.h"
#include <stdint.h>
#include <stddef.h>
#include "sys.h"

void* malloc(size_t sz);
void free(void* addr);

void exit();


#endif
