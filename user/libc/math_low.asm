[bits 32]
section .text

global sqrt_int
sqrt_int:
    push ebp
    mov ebp, esp
    add ebp, 8
    fld qword [ebp]
    fwait
    fsqrt
    fwait
    fst qword [sqrt_int_buf]
    fwait
    mov eax, dword [sqrt_int_buf]
    mov edx, dword [sqrt_int_buf + 4]
    pop ebp
    ret

sqrt_int_buf: dq 0 

global sin_int
sin_int:
    push ebp
    mov ebp, esp
    add ebp, 8
    fld qword [ebp]
    fwait
    fsin
    fwait
    fst qword [sin_int_buf]
    fwait
    mov eax, dword [sin_int_buf]
    pop ebp
    ret

sin_int_buf: dd 0 

global fabs_int
fabs_int:
    push ebp
    mov ebp, esp
    add ebp, 8
    fld dword [ebp]
    fwait
    fabs
    fwait
    fst dword [fabs_int_buf]
    fwait
    mov eax, dword [fabs_int_buf]
    pop ebp
    ret

fabs_int_buf: dd 0 

global cos_int
cos_int:
    push ebp
    mov ebp, esp
    add ebp, 8
    fld dword [ebp]
    fwait
    fcos
    fwait
    fst dword [cos_int_buf]
    fwait
    mov eax, dword [cos_int_buf]
    pop ebp
    ret

cos_int_buf: dd 0 

global fmul_int
fmul_int:
    push ebp
    mov ebp, esp
    add ebp, 8
    fld dword [ebp]
    fwait
    add ebp, 4
    fmul dword [ebp]
    fwait
    fst dword [fmul_int_buf]
    fwait
    mov eax, dword [fmul_int_buf]
    pop ebp
    ret

fmul_int_buf: dd 0