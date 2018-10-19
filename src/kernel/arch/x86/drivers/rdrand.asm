[bits 32]
section .text

detect_rdrand:
    mov eax, 1
    cpuid
    bt ecx, 30
    jnc no_rdrand
    mov byte [rdrand_det], 2
    jmp rdrand_det_success

global rdrand_int
rdrand_int:
    cmp byte [rdrand_det], 1
    je no_rdrand
    cmp byte [rdrand_det], 0
    je detect_rdrand
rdrand_det_success:
    mov edx, 20

    rdrand eax
    jnc rdrand_det_success
    dec edx
    cmp edx, 10
    jle rdrand_failed

    ret

rdrand_failed:
    mov eax, 0
    ret

no_rdrand:
    mov eax, 0
    mov byte [rdrand_det], 1
    ret

section .data
rdrand_det: db 0 ; 0 = not tested ; 1 = no rdrdrand ; 2 = can read