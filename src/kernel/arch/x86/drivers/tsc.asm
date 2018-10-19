[bits 32]
section .text

detect_tsc:
    mov eax, 1
    cpuid
    bt edx, 4
    jnc no_tsc
    mov byte [tsc_det], 2
    jmp tsc_det_success

global get_cpu_tsc
get_cpu_tsc:
    cmp byte [tsc_det], 1
    je no_tsc
    cmp byte [tsc_det], 0
    je detect_tsc
tsc_det_success:
    rdtsc
    ret

no_tsc:
    mov eax, 0
    mov byte [tsc_det], 1
    ret

section .data
tsc_det: db 0 ; 0 = not tested ; 1 = no rdtsc ; 2 = can read