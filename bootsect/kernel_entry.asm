[bits 32]

global _start;
_start:
  ;[extern kernel_main]
  ;call kernel_main
  [extern _kernel_early]
  call _kernel_early
  jmp $
