[bits 32]

section .data
pid: db 0
argc: dd 0
argv: dd 0

section .text

extern main
global _start
_start:
    xor eax, eax
    mov ebx, 3
    xor ecx, ecx
    xor edx, edx
    int 0x80

    mov eax, 1
    mov ebx, 3
    mov ecx, 15
    xor edx, edx
    int 0x80

    xor eax, eax
    mov ebx, 7
    xor ecx, ecx
    xor edx, edx
    int 0x80
    mov byte [pid], al

    xor eax, eax
    mov ebx, 10
    xor ecx, ecx
    mov cl, byte [pid]
    xor edx, edx
    int 0x80
    mov dword [argv], eax

    xor eax, eax
    mov ebx, 12
    xor ecx, ecx
    mov cl, byte [pid]
    xor edx, edx
    int 0x80
    mov dword [argc], eax

    push dword [argv]
    push dword [argc]

    call main

    xor eax, eax
    mov ebx, 14
    xor ecx, ecx
    mov cl, byte [pid]
    xor edx, edx
    int 0x80
    jmp $