#pragma once

#define DEBUG

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <rhino/panic.h>
#include <rhino/debug.h>

#define KERNEL_VBASE 0xC0000000

#define BIT_IS_SET(var, pos) (((uint64_t)var) & (1ULL << (pos)))
#define BIT_IS_CLEAR(var, pos) (!(((uint64_t)var) & (1ULL << (pos))))
#define BIT_SET(var, pos) ((var) |= (1ULL << (pos)))
#define BIT_CLEAR(var, pos) ((var) &= ~(1ULL << (pos)))

#define ASSERT(b) ((b) ? (void)0 : panic_assert(__FILE__, (char *)__func__, __LINE__, #b))
#define PANIC_M(x) (panic_m(x, __FILE__, (char *)__func__, __LINE__))
#define panic() (panic_int(__FILE__, (char *)__func__, __LINE__))

#define CLI() asm("cli")
#define STI() asm("sti")

#define UNUSED(x) (void)(x)

#define IDIV_CEIL(x, y) (((x) + (y)-1) / (y))
#define IDIV_CEIL_NO_OVERFLOW(x, y) (1 + (((x)-1) / (y)))

// MIN and MAX are so complicated because of double evaluation see https://stackoverflow.com/questions/3437404/min-and-max-in-c
#define MIN(a, b) \
    ({typeof(a) _a = (a); \
      typeof(b) _b = (b); \
      ((_a < _b) ? (_a) : (_b)); })

#define MAX(a, b) \
    ({typeof(a) _a = (a); \
      typeof(b) _b = (b); \
      ((_a > _b) ? (_a) : (_b)); })

uint32_t save_eflags();
void load_eflags(uint32_t eflags);

struct GUID
{
    uint8_t bytes[16];
};