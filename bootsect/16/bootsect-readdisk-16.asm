disk_load:
	pusha
	push dx
	mov ah, 0x02
	mov al, dh
	mov cl, 0x02
	mov ch, 0x00
	mov dh, 0x00
	int 0x13
	jc disk_error

	pop dx
	cmp al, dh
	jne sectors_error
	popa
	ret

disk_error:
	mov bx, DISK_ERROR
	call print_bootsec
	call print_bootsec_nl
	mov dh, ah
	call bootsec_print_hex
	jmp disk_loop

sectors_error:
	mov bx, SECTORS_ERROR
	call print_bootsec
	call print_bootsec_nl

disk_loop:
	jmp $

DISK_ERROR: db "Disk read error", 0
SECTORS_ERROR: db "Incorrect number of sectors read", 0
