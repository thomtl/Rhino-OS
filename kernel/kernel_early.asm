[section .text]

[global _kernel_early]
_kernel_early:
  [extern kprint]
  push KERNEL_ENTER_COMPLETE
  call kprint
  pop eax
  mov eax, 0
  [extern kernel_main]
  call kernel_main
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
