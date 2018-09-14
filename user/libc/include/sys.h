#ifndef RHINO_LIBC_SYS_H
#define RHINO_LIBC_SYS_H
#include "cdefs.h"
#include <stdint.h>

uint32_t syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

#endif