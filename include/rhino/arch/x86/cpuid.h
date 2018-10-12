#pragma once

#include <stdint.h>

enum {
    CPUID_FEAT_ECX_SSE3         = 0, 
    CPUID_FEAT_ECX_PCLMUL       = 1,
    CPUID_FEAT_ECX_DTES64       = 2,
    CPUID_FEAT_ECX_MONITOR      = 3,  
    CPUID_FEAT_ECX_DS_CPL       = 4,  
    CPUID_FEAT_ECX_VMX          = 5,  
    CPUID_FEAT_ECX_SMX          = 6,  
    CPUID_FEAT_ECX_EST          = 7,  
    CPUID_FEAT_ECX_TM2          = 8,  
    CPUID_FEAT_ECX_SSSE3        = 9,  
    CPUID_FEAT_ECX_CID          = 10,
    CPUID_FEAT_ECX_SDBG         = 11,
    CPUID_FEAT_ECX_FMA          = 12,
    CPUID_FEAT_ECX_CX16         = 13, 
    CPUID_FEAT_ECX_ETPRD        = 14, 
    CPUID_FEAT_ECX_PDCM         = 15, 
    CPUID_FEAT_ECX_PCIDE        = 17, 
    CPUID_FEAT_ECX_DCA          = 18, 
    CPUID_FEAT_ECX_SSE4_1       = 19, 
    CPUID_FEAT_ECX_SSE4_2       = 20, 
    CPUID_FEAT_ECX_x2APIC       = 21, 
    CPUID_FEAT_ECX_MOVBE        = 22, 
    CPUID_FEAT_ECX_POPCNT       = 23, 
    CPUID_FEAT_ECX_TSCDEADLINE  = 24,
    CPUID_FEAT_ECX_AES          = 25, 
    CPUID_FEAT_ECX_XSAVE        = 26, 
    CPUID_FEAT_ECX_OSXSAVE      = 27, 
    CPUID_FEAT_ECX_AVX          = 28,
    CPUID_FEAT_ECX_F16C         = 29,
    CPUID_FEAT_ECX_RDRAND       = 30,
 
    CPUID_FEAT_EDX_FPU          = 0,  
    CPUID_FEAT_EDX_VME          = 1,  
    CPUID_FEAT_EDX_DE           = 2,  
    CPUID_FEAT_EDX_PSE          = 3,  
    CPUID_FEAT_EDX_TSC          = 4,  
    CPUID_FEAT_EDX_MSR          = 5,  
    CPUID_FEAT_EDX_PAE          = 6,  
    CPUID_FEAT_EDX_MCE          = 7,  
    CPUID_FEAT_EDX_CX8          = 8,  
    CPUID_FEAT_EDX_APIC         = 9,  
    CPUID_FEAT_EDX_SEP          = 11, 
    CPUID_FEAT_EDX_MTRR         = 12, 
    CPUID_FEAT_EDX_PGE          = 13, 
    CPUID_FEAT_EDX_MCA          = 14, 
    CPUID_FEAT_EDX_CMOV         = 15, 
    CPUID_FEAT_EDX_PAT          = 16, 
    CPUID_FEAT_EDX_PSE36        = 17, 
    CPUID_FEAT_EDX_PSN          = 18, 
    CPUID_FEAT_EDX_CLF          = 19, 
    CPUID_FEAT_EDX_DTES         = 21, 
    CPUID_FEAT_EDX_ACPI         = 22, 
    CPUID_FEAT_EDX_MMX          = 23, 
    CPUID_FEAT_EDX_FXSR         = 24, 
    CPUID_FEAT_EDX_SSE          = 25, 
    CPUID_FEAT_EDX_SSE2         = 26, 
    CPUID_FEAT_EDX_SS           = 27, 
    CPUID_FEAT_EDX_HTT          = 28, 
    CPUID_FEAT_EDX_TM1          = 29, 
    CPUID_FEAT_EDX_IA64         = 30,
    CPUID_FEAT_EDX_PBE          = 31
};

#define CPUID_GET_VENDOR_ID 0x0
#define CPUID_GET_FEATURES 0x1

void cpuid(uint32_t code, uint32_t *a, uint32_t *c, uint32_t *d);
void id_cpu();