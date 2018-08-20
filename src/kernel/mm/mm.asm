[bits 32]
[section .text]
[global tlb_flush]
tlb_flush:
    pusha
    mov eax, cr3
    mov cr3, eax
    popa
    ret