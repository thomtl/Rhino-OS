[org 0x7c00]
KERNEL_OFFSET equ 0x1000
mov [BOOT_DRIVE], dl
mov bp, 0x9000
mov sp, bp

mov bx, MSG_BOOTING
call print_bootsec
call print_bootsec_nl

mov bx, MSG_REAL_MODE
call print_bootsec
call print_bootsec_nl

;jmp a20_failed
;call check_a20
;cmp ax, 1
;je a20_enabled

;in al, 0x92
;or al, 2
;out 0x92, al

;call check_a20
;cmp ax, 1
;je a20_enabled
;jmp a20_failed

in al, 0x92
test al, 2
jnz a20_enabled
or al, 2
and al, 0xFE
out 0x92, al
jmp a20_enabled

a20_enabled:
	call load_kernel
	call switch_to_pm

jmp $
;%include "bootsect/16/bootsect-checka20-16.asm"
%include "bootsect/16/bootsect-print-16.asm"
%include "bootsect/16/bootsect-printhex-16.asm"
%include "bootsect/16/bootsect-readdisk-16.asm"
%include "bootsect/32/bootsect-gdt-32.asm"
%include "bootsect/32/bootsect-print-32.asm"
%include "bootsect/32/bootsect-enableprot-32.asm"

[bits 16]
load_kernel:
	mov bx, MSG_LOADING_KERNEL
	call print_bootsec
	call print_bootsec_nl

	mov bx, KERNEL_OFFSET
	mov dh, 32
	mov dl, [BOOT_DRIVE]
	call disk_load
	ret

[bits 32]
BEGIN_PM:
	mov ebx, MSG_PROT_MODE
	call print
	call KERNEL_OFFSET
	jmp $

BOOT_DRIVE:
	db 0
MSG_REAL_MODE:
 	db 'Started in 16bit mode', 0
MSG_PROT_MODE:
 	db "Loaded 32bit mode", 0
MSG_BOOTING:
	db 'Booting BOOTOS', 0
MSG_LOADING_KERNEL:
	db "Loading kernel", 0
MSG_A20_ERROR:
	db "Error Enabling A20 Line", 0
times 510-($-$$) db 0
dw 0xaa55
