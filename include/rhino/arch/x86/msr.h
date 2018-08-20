#pragma once

#include <stdint.h>
#include <rhino/arch/x86/cpuid.h>
#define CPUID_MSR_FLAG 1 << 5

uint32_t msr_check();
void msr_read(uint32_t msr, uint32_t *lo, uint32_t *hi);
void msr_write(uint32_t msr, uint32_t lo, uint32_t hi);