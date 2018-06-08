[bits 32]
[section .text]
global enable_paging
enable_paging:
  mov eax, cr0
  or eax, 0x80000001
  mov cr0, eax
  ret
