#include <rhino/arch/x86/cpuid.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <libk/stdio.h>
inline void cpuid(uint32_t code, uint32_t *a, uint32_t *c, uint32_t *d){
    asm("cpuid" : "=a"(*a), "=c"(*c), "=d"(*d) : "a"(code) : "ebx");
}

static inline int cpuid_string(int code, uint32_t where[4]) {
  asm volatile("cpuid":"=a"(*where),"=b"(*(where+1)),
               "=c"(*(where+2)),"=d"(*(where+3)):"a"(code));
  return (int)where[0];
}

void id_cpu(){
    uint32_t eax, ecx, edx;
    uint32_t buf[4] = {0};
    cpuid_string(CPUID_GET_VENDOR_ID, buf);

    kprint("VendorID: ");
    putchar((buf[1] & 0xFF));
    putchar(((buf[1] >> 8) & 0xFF));
    putchar(((buf[1] >> 16) & 0xFF));
    putchar(((buf[1] >> 24) & 0xFF));

    putchar((buf[3] & 0xFF));
    putchar(((buf[3] >> 8) & 0xFF));
    putchar(((buf[3] >> 16) & 0xFF));
    putchar(((buf[3] >> 24) & 0xFF));

    putchar((buf[2] & 0xFF));
    putchar(((buf[2] >> 8) & 0xFF));
    putchar(((buf[2] >> 16) & 0xFF));
    putchar(((buf[2] >> 24) & 0xFF));
    putchar('\n');

    cpuid(CPUID_GET_MAXIMUM_EXTENDED, &eax, &ecx, &edx);
    if(eax >= 0x80000004){
        cpuid_string(0x80000002, buf);
        kprint("CPU Model: ");
        for(int i = 0; i < 4; i++){
            putchar((buf[i] & 0xFF));
            putchar(((buf[i] >> 8) & 0xFF));
            putchar(((buf[i] >> 16) & 0xFF));
            putchar(((buf[i] >> 24) & 0xFF)); 
        }
        cpuid_string(0x80000003, buf);
        for(int i = 0; i < 4; i++){
            putchar((buf[i] & 0xFF));
            putchar(((buf[i] >> 8) & 0xFF));
            putchar(((buf[i] >> 16) & 0xFF));
            putchar(((buf[i] >> 24) & 0xFF)); 
        }
        cpuid_string(0x80000004, buf);
        for(int i = 0; i < 4; i++){
            putchar((buf[i] & 0xFF));
            putchar(((buf[i] >> 8) & 0xFF));
            putchar(((buf[i] >> 16) & 0xFF));
            putchar(((buf[i] >> 24) & 0xFF));
        }
        putchar('\n');
    }

    printf("Features: ");
    cpuid(CPUID_GET_FEATURES, &eax, &ecx, &edx);
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_FPU)) kprint("FPU ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_VME)) kprint("VME ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_DE)) kprint("DE ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_PSE)) kprint("PSE ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_TSC)) kprint("TSC ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_MSR)) kprint("MSR ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_PAE)) kprint("PAE ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_MCE)) kprint("MCE ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_CX8)) kprint("CX8 ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_APIC)) kprint("APIC ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_SEP)) kprint("SEP ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_MTRR)) kprint("MTRR ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_PGE)) kprint("PGE ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_MCA)) kprint("MCA ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_CMOV)) kprint("CMOV ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_PAT)) kprint("PAT ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_PSE36)) kprint("PSE-36 ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_PSN)) kprint("PSN ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_CLF)) kprint("CLFLUSH ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_DTES)) kprint("DS ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_ACPI)) kprint("ACPI ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_MMX)) kprint("MMX ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_SSE)) kprint("SSE ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_SSE2)) kprint("SSE2 ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_SS)) kprint("SS ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_HTT)) kprint("HTT ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_TM1)) kprint("TM ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_IA64)) kprint("IA64 ");
    if(BIT_IS_SET(edx, CPUID_FEAT_EDX_PBE)) kprint("PBE ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_SSE3)) kprint("SSE3 ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_PCLMUL)) kprint("PCLMUL ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_DTES64)) kprint("DTES64 ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_DS_CPL)) kprint("DS-CPL ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_VMX)) kprint("VMX ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_SMX)) kprint("SMX ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_EST)) kprint("EST ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_TM2)) kprint("TM2 ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_SSSE3)) kprint("SSE3 ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_CID)) kprint("CID ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_SDBG)) kprint("SDBG ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_FMA)) kprint("FMA ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_CX16)) kprint("CX16 ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_ETPRD)) kprint("XTPTR ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_PDCM)) kprint("PDCM ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_PCIDE)) kprint("PCID ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_DCA)) kprint("DCA ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_SSE4_1)) kprint("SSE4.1 ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_SSE4_2)) kprint("SSE4.2 ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_x2APIC)) kprint("X2APIC ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_MOVBE)) kprint("MOVBE ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_POPCNT)) kprint("POPCNT ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_TSCDEADLINE)) kprint("TSC-DEADLINE ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_AES)) kprint("AES ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_XSAVE)) kprint("XSAVE ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_OSXSAVE)) kprint("OSXSAVE ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_AVX)) kprint("AVX ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_F16C)) kprint("F16C ");
    if(BIT_IS_SET(ecx, CPUID_FEAT_ECX_RDRAND)) kprint("RDRAND ");
    
    putchar('\n');
}