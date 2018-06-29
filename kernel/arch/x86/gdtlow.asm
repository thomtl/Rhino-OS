[bits 32]
[section .text]
extern gdtptr
global gdt_flush
gdt_flush:
  lgdt [gdtptr]
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  jmp 0x08:flush2

flush2:
  ret
