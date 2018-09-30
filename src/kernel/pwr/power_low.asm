[bits 32]
[section .text]

; Bad 8042 CPU reset / reboot code

[global reboot]
reboot:
  in al, 0x64
  test al, 00000010b
  jne reboot

  mov al, 0xFE
  out 0x64, al
