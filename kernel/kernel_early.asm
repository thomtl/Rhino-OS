[bits 32]
; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)

[section .data]
align 0x1000
boot_page_directory:
	dd 0x00000083
	times (KERNEL_PAGE_NUMBER - 1) dd 0

	dd 0x00000083
	times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0  ; Pages after the

; Declare a multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at a
; 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.
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
	mov ecx, (boot_page_directory - KERNEL_VIRTUAL_BASE)
	mov cr3, ecx

	mov ecx, cr4
	or ecx, 0x00000010
	mov cr4, ecx


	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx

	lea ecx, [_load_kernel]
	jmp ecx

_load_kernel:
	; Unmap the first 4mb
	mov dword [boot_page_directory], 0
	invlpg [0]

	; Setup the stack
	mov esp, stack_top

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

;_kernel_end:
