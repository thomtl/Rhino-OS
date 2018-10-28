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