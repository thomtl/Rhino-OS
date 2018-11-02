[bits 32]
section .text
no:
    mov eax, 0
    ret

global detect_mmx
detect_mmx:
    mov eax, 1
    cpuid
    bt edx, 23
    jnc no
    mov eax, 1
    ret

global detect_sse
detect_sse:
    mov eax, 1
    cpuid
    bt edx, 25
    jnc no
    mov eax, 1
    ret

global detect_sse2
detect_sse2:
    mov eax, 1
    cpuid
    bt edx, 26
    jnc no
    mov eax, 1
    ret

global detect_avx
detect_avx:
    mov eax, 1
    cpuid
    bt ecx, 28
    jnc no
    mov eax, 1
    ret

global detect_avx_512
detect_avx_512:
    mov eax, 0
    cpuid
    cmp eax, 7
    jl no

    mov eax, 7
    cpuid
    bt ebx, 16
    jnc no

    mov eax, 1
    ret