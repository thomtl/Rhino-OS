[bits 32]
[global _start]
[extern main]
[extern printf]
_start:
  cli
  cld
  sti

  push hello
  call printf

  call main

  push stop
  call printf

  jmp $

hello:
  db 'Starting TSHELL', 0

stop:
  db 'Stopping TSHELL', 0
