[section .text]

[global _kernel_early]
_kernel_early:
  ; Print that we have successfully jumped to the kernel
  [extern kprint]
  push KERNEL_ENTER_COMPLETE
  call kprint
  pop eax
  mov eax, 0
  ; Jump to the C Kernel
  [extern kernel_main]
  call kernel_main
  ; Critical Error the kernel returned print an error on the display and hlt the PC
  push KERNEL_EXIT_ERROR
  call kprint
  pop eax
  mov eax, 0
  jmp START_EXIT_LOOP

START_EXIT_LOOP:
  cli
EXIT_LOOP:
  hlt
  jmp EXIT_LOOP

KERNEL_ENTER_COMPLETE:
  db "Entered Kernel!", 0x0a, 0

KERNEL_EXIT_ERROR:
  db "Critical Error kernel exited halting!", 0x0a, 0
