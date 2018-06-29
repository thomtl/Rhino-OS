[bits 32]
[section .text]

main:
    mov eax, M
    int 0x80
    ret

M:
  db 'Hello World from an external program!', 0
