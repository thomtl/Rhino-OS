[bits 32]
[section .text]
[global tlb_flush]
tlb_flush:
    pusha
    mov eax, cr3
    mov cr3, eax
    popa
    ret

[global set_cr3]
set_cr3:
    push ebp
    mov ebp, esp

    add ebp, 8

    mov eax, dword [ebp]
    mov cr3, eax

    pop ebp
    ret