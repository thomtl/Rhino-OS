[bits 32]
section .text

global switch_to_usermode
switch_to_usermode:
    cli
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp
    push dword 0x23
    push eax
    pushf
    pop eax
    bts eax, 9
    push eax
    push 0x1B
    push user_addr
    iret
user_addr:
    ret

