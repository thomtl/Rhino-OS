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
    fsin
    fst qword [sin_int_buf]
    fwait
    mov eax, dword [sin_int_buf]
    mov edx, dword [sin_int_buf + 4]
    pop ebp
    ret

sin_int_buf: dq 0 

global fabs_int
fabs_int:
    push ebp
    mov ebp, esp
    add ebp, 8
    fld qword [ebp]
    fwait
    fabs
    fwait
    fst qword [fabs_int_buf]
    fwait
    mov eax, dword [fabs_int_buf]
    mov edx, dword [fabs_int_buf + 4]
    pop ebp
    ret

fabs_int_buf: dq 0 

global cos_int
cos_int:
    push ebp
    mov ebp, esp
    add ebp, 8
    fld qword [ebp]
    fwait
    fcos
    fwait
    fst qword [cos_int_buf]
    fwait
    mov eax, dword [cos_int_buf]
    mov edx, dword [cos_int_buf + 4]
    pop ebp
    ret

cos_int_buf: dq 0 

global fmul_int
fmul_int:
    push ebp
    mov ebp, esp
    add ebp, 8
    fld qword [ebp + 8]
    fmul qword [ebp]
    fst qword [fmul_int_buf]
    fwait
    mov eax, dword [fmul_int_buf]
    mov edx, dword [fmul_int_buf + 4]
    pop ebp
    ret

fmul_int_buf: dq 0

global fdiv_int
fdiv_int:
    push ebp
    mov ebp, esp
    add ebp, 8
    fld qword [ebp + 8]
    fdiv qword [ebp]
    fst qword [fdiv_int_buf]
    fwait
    mov eax, dword [fdiv_int_buf]
    mov edx, dword [fdiv_int_buf + 4]
    pop ebp
    ret

fdiv_int_buf: dq 0