_kernel_start:
[section .bootstrap_stack]
stack_bottom:
  align 16
  resb 16384
stack_top:
[section .bss]
  align 4096
boot_pagedir:
  ;[skip 4096]
  resb 4096
boot_pagetab1:
  ;[skip 4096]
  resb 4096
[section .text]


[global _kernel_early:function (_kernel_early.end - _kernel_early)]
_kernel_early:
  ; Print that we have successfully jumped to the kernel
  [extern kprint]
  push KERNEL_ENTER_COMPLETE
  call kprint
  pop eax
  mov eax, 0
  ; initialize cpu
  mov edi, [boot_pagetab1 - 0xC0000000]
  mov esi, 0
  mov ecx, 1023
a:
  cmp esi, (_kernel_start - 0xC0000000)
  jl b
  cmp esi, (_kernel_end - 0xC0000000)
  jge c

  mov edx, esi
  or edx, 0x003
  mov [esi], edx
b:
  add esi, 4096
  add edi, 4
  loop b
c:
  mov long [boot_pagetab1 - 0xC0000000 + 1023 * 4], (0x000B8000 | 0x003)
  mov long [boot_pagedir - 0xC0000000 + 0], (boot_pagetab1 - 0xC0000000 + 0x003)
  mov long [boot_pagedir - 0xC0000000 + 768 * 4], (boot_pagetab1 - 0xC0000000 + 0x003)
  mov ecx, (boot_pagedir - 0xC0000000)
  mov cr3, ecx

  mov ecx, cr0
  or ecx, 0x80010000
  mov cr0, ecx
  lea ecx, [d]
  jmp ecx

d:
  mov long [boot_pagedir + 0], 0
  mov ecx, cr3
  mov cr3, ecx

  mov esp, stack_top
  ; Jump to the C Kernel
  [extern kernel_main]
  call kernel_main
  ; Critical Error the kernel returned print an error on the display and hlt the PC
  push KERNEL_EXIT_ERROR
  call kprint
  pop eax
  mov eax, 0
  jmp START_EXIT_LOOP
_kernel_early.end:
START_EXIT_LOOP:
  cli
EXIT_LOOP:
  hlt
  jmp EXIT_LOOP

KERNEL_ENTER_COMPLETE:
  db "Entered Kernel!", 0x0a, 0

KERNEL_EXIT_ERROR:
  db "Critical Error kernel exited halting!", 0x0a, 0
_kernel_end:
