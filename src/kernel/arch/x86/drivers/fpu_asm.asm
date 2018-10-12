[bits 32]
section .text

global probe_fpu
probe_fpu:
    mov eax, cr0
    bt eax, 2
    jc fpu_not_detected
    bt eax, 4
    jnc fpu_not_detected

    btr eax, 2
    btr eax, 4
    mov cr0, eax

    fninit
    fnstsw [fpu_test_word]
    cmp word [fpu_test_word], 0
    mov cr0, eax
    jne fpu_not_detected

    mov eax, 1
    ret


fpu_not_detected:
    mov eax, 0
    ret

global enable_native_interrupt_fpu
enable_native_interrupt_fpu:
    mov eax, cr0
    bts eax, 5
    mov cr0, eax
    ret

global enable_fxsave
enable_fxsave:
    mov eax, cr4
    bts eax, 9
    mov cr4, eax
    ret

global init_fpu_low
init_fpu_low:
    call enable_native_interrupt_fpu
    call enable_fxsave
    finit
    fclex
    ret

global fpu_get_status_word
fpu_get_status_word:
    xor eax, eax
    fstsw ax
    ret

fpu_test_word: dw 0xAA55