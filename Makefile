C_SOURCES = $(wildcard kernel/*.c kernel/mm/*.c kernel/types/*.c kernel/fs/*.c kernel/security/*.c kernel/multitasking/*.c kernel/power/*.c kernel/user/*.c drivers/*.c kernel/arch/x86/*.c libk/*.c libk/string/*.c libk/stdio/*.c libk/stdlib/*.c)
HEADERS = $(wildcard kernel/*.h kernel/mm/*.h  kernel/types/*.h kernel/fs/*.h kernel/security/*.h kernel/multitasking/*.h kernel/power/*.h kernel/user/*.h drivers/*.h kernel/arch/x86/*.h libk/*.h libk/include/*.h libk/include/sys/*.h)

# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o kernel/arch/x86/interrupt.o kernel/arch/x86/gdtlow.o kernel/multitasking/switch.o kernel/mm/mm.o kernel/power/power_low.o}

# Change this if your cross-compiler is somewhere else
CC = i686-elf-gcc

# -g: Use debugging symbols in gcc
CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=c99
.PHONY: all clean run bochs initrd
all: run
# First rule is run by default

initrd:
	rm -f initrd.img
	./utils/initrdgen/genrd ./utils/initrdgen/test.txt test.txt ./utils/initrdgen/test1.txt test1.txt ./utils/UserTest/usertest test.prg ./utils/TSHELL/tshell init.prg

rhino.iso: kernel.bin initrd
	mkdir -p build/sys/boot/grub
	cp kernel.bin build/sys/boot/kernel.bin
	cp initrd.img build/sys/boot/initrd.img
	cp build/grub.cfg build/sys/boot/grub/grub.cfg
	grub-mkrescue -o ./rhino.iso build/sys

kernel.bin: kernel/kernel_early.o ${OBJ}
	${CC} -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

run: rhino.iso
	DISPLAY=:0 qemu-system-x86_64 -m 512M -cdrom rhino.iso -d cpu_reset -D log/qemulog
	rm -rf build/sys

bochs: rhino.iso
	DISPLAY=:0 bochs
	rm -rf build/sys

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	yasm $< -f elf -o $@ -p nasm
%.o: %.s
	yasm $< -f elf -o $@ -p gas
	
clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf *.iso *.out
	rm -rf kernel/*.o kernel/mm/*.o kernel/types/*.o kernel/fs/*.o kernel/security/*.o kernel/multitasking/*.o kernel/power/*.o kernel/user/*.o boot/*.bin drivers/*.o kernel/arch/x86/*.o libk/*.o libk/string/*.o libk/stdio/*.o libk/stdlib/*.o
	rm -rf build/sys
	rm -rf log/*
	rm -f initrd.img
	rm -f snapshot.txt
