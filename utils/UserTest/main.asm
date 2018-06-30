[bits 32]
[section .text]

main:
    mov eax, 1
    mov ebx, 1
    mov ecx, 'Q'
    int 0x80
    ret
