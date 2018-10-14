[bits 32]
section .text

global init_sse
init_sse:
    call detect_sse
    cmp eax, 0
    je no_sse

    mov eax, cr0
    and ax, 0xFFFB
    or ax, 0x2
    mov cr0, eax

    mov eax, cr4
    bts eax, 9 ; Set OSFXSR bit
    bts eax, 10 ; Set OSXMMEXCPT bit
    mov cr4, eax

    mov eax, 1
    ret

global detect_sse
detect_sse:
    mov eax, 1
    cpuid
    bt edx, 25
    jnc no_sse
    mov eax, 1
    ret

no_sse:
    mov eax, 0
    ret