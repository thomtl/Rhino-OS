#pragma once

#include <rhino/common.h>
#include <rhino/arch/x86/cpuid.h>

#define IA32_MISC_ENABLE 0x1A0
#define APIC_BASE 0x1B

uint32_t msr_check();
void msr_read(uint32_t msr, uint32_t *lo, uint32_t *hi);
void msr_write(uint32_t msr, uint32_t lo, uint32_t hi);