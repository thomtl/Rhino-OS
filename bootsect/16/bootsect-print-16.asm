print_bootsec:
	pusha

print_bootsec_start:
	mov al, [bx]
	cmp al, 0
  je print_bootsec_done

	mov ah, 0x0e
	int 0x10

	add bx, 1
	jmp print_bootsec_start

print_bootsec_done:
	popa
	ret

print_bootsec_nl:
	pusha
	mov ah, 0x0e
	mov al, 0x0a
	int 0x10
	mov al, 0x0d
	int 0x10
	popa
  ret
