[bits 32]
section .text

global get_cpu_tsc
get_cpu_tsc:
    mov eax, 1
    cpuid
    bt edx, 4
    jnc no_tsc
    rdtsc
    ret

no_tsc:
    mov eax, 0
    ret