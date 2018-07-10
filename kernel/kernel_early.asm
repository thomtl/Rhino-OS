[bits 32]
; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

[section .bss]
align 16
stack_bottom:
  resb 16384
stack_top:

[section .text]
align 4
global _kernel_early
_kernel_early:

	; Setup the stack
	mov esp, stack_top

	;Send the multiboot stuff to the kernel
	push eax
	push ebx

	; Jump to the C Kernel
	[extern kernel_main]
	call kernel_main

	; Critical Error the kernel returned hlt the PC
	jmp START_EXIT_LOOP

START_EXIT_LOOP:
  cli
EXIT_LOOP:
  hlt
  jmp EXIT_LOOP
