#ifndef CPUID_H
#define CPUID_H
#include <stdint.h>
void cpuid(uint32_t code, uint32_t *a, uint32_t *d);
#endif