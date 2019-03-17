[bits 32]

section .data
pid: db 0
argc: dd 0
argv: dd 0

section .text

extern main
global _start
_start:
    mov eax, 2
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    int 0x80

    mov eax, 15
    mov ebx, 15
    xor ecx, ecx
    xor edx, edx
    int 0x80

    mov eax, 3
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    int 0x80
    mov byte [pid], al

    mov eax, 6 ; Here
    xor ebx, ebx
    mov bl, byte [pid]
    xor ecx, ecx
    xor edx, edx
    int 0x80
    mov dword [argv], eax

    mov eax, 8
    xor ebx, ebx
    mov bl, byte [pid]
    xor ecx, ecx
    xor edx, edx
    int 0x80
    mov dword [argc], eax

    push dword [argv]
    push dword [argc]

    call main

    mov eax, 10
    xor ebx, ebx
    mov bl, byte [pid]
    xor ecx, ecx
    xor edx, edx
    int 0x80
    jmp $