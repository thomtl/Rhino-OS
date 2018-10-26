#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <rhino/panic.h>
#include <rhino/debug.h>

#define KERNEL_VBASE 0xC0000000

#define BIT_IS_SET(var, pos) ((var) & (1 << (pos)))
#define BIT_SET(var, pos) ((var) |= (1ULL << (pos)))
#define BIT_CLEAR(var, pos) ((var) &= ~(1ULL << (pos)))
#define ASSERT(b) ((b) ? (void)0 : panic_assert(__FILE__, (char*)__func__, __LINE__, #b))
#define PANIC_M(x) (panic_m(x, __FILE__, (char*)__func__, __LINE__))
#define panic() (panic_int(__FILE__, (char*)__func__, __LINE__))
#define CLI() asm("cli")
#define STI() asm("sti")
#define UNUSED(x) (void)(x)

#define DEBUG