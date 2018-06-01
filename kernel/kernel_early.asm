; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

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
[global _kernel_early:function (_kernel_early.end - _kernel_early)]
_kernel_early:
  ; Print that we have successfully jumped to the kernel
  mov esp, stack_top

  ; Jump to the C Kernel
  [extern kernel_main]
  call kernel_main

  ; Critical Error the kernel returned print an error on the display and hlt the PC
  jmp START_EXIT_LOOP
.end:

START_EXIT_LOOP:
  cli
EXIT_LOOP:
  hlt
  jmp EXIT_LOOP

;_kernel_end:
