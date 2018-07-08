#ifndef STACK_SMASH_H
#define STACK_SMASH_H
#include <stdint.h>
#include "../common.h"
#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0x2edee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif

__attribute__((noreturn))
void __stack_chk_fail(void);
#endif
