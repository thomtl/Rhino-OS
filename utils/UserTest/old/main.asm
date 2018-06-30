[bits 32]
[section .text]

main:
  mov eax, 1
  mov ebx, 1
  mov ecx, 'H'
  int 0x80
  mov ecx, 'e'
  int 0x80
  mov ecx, 'l'
  int 0x80
  mov ecx, 'l'
  int 0x80
  mov ecx, 'o'
  int 0x80
  mov ecx, ' '
  int 0x80
  mov ecx, 'W'
  int 0x80
  mov ecx, 'o'
  int 0x80
  mov ecx, 'r'
  int 0x80
  mov ecx, 'l'
  int 0x80
  mov ecx, 'd'
  int 0x80
  ret
