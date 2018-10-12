[bits 32]
section .text

global sqrt_int
sqrt_int:
    push ebp
    mov ebp, esp
    add ebp, 8
    fld dword [ebp]
    fwait
    fsqrt
    fwait
    fst dword [sqrt_int_buf]
    fwait
    mov eax, dword [sqrt_int_buf]
    pop ebp
    ret

sqrt_int_buf: dd 0 

global sin_int
sin_int:
    push ebp
    mov ebp, esp
    add ebp, 8
    fld dword [ebp]
    fwait
    fsin
    fwait
    fst dword [sin_int_buf]
    fwait
    mov eax, dword [sin_int_buf]
    pop ebp
    ret

sin_int_buf: dd 0 