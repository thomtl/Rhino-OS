C_SOURCES = $(wildcard kernel/*.c kernel/heap/*.c kernel/paging/*.c kernel/types/*.c kernel/fs/*.c kernel/security/*.c kernel/multitasking/*.c kernel/user/*.c drivers/*.c cpu/*.c libc/*.c libc/string/*.c libc/stdio/*.c libc/stdlib/*.c)
HEADERS = $(wildcard kernel/*.h kernel/heap/*.h kernel/paging/*.h  kernel/types/*.h kernel/fs/*.h kernel/security/*.h kernel/multitasking/*.h kernel/user/*.h drivers/*.h cpu/*.h libc/*.h libc/include/*.h libc/include/sys/*.h)

# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o cpu/gdtlow.o kernel/multitasking/switch.o}

# Change this if your cross-compiler is somewhere else
CC = i686-elf-gcc

# -g: Use debugging symbols in gcc
CFLAGS = -g -m32 -fno-builtin -fstack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=c99

all: rhino.iso run
# First rule is run by default

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case

rhino.iso: kernel.bin
	mkdir -p build/sys/boot/grub
	cp kernel.bin build/sys/boot/kernel.bin
	cp initrd.img build/sys/boot/initrd.img
	cp utils/UserTest/usertest build/sys/boot/usertest.prg
	cp build/grub.cfg build/sys/boot/grub/grub.cfg
	grub-mkrescue -o ./rhino.iso build/sys --verbose

kernel.bin: kernel/kernel_early.o ${OBJ}
	#;i686-elf-ld -T linker.ld -o $@  -Ttext 0x1000 $^ #--oformat binary
	${CC} -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

run:
	DISPLAY=:0 qemu-system-x86_64 -m 16M -cdrom rhino.iso -d cpu_reset -D log/qemulog
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
%.bin: %.asm
	nasm $< -f elf -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf *.iso *.out
	rm -rf kernel/*.o kernel/heap/*.o kernel/paging/*.o kernel/types/*.o kernel/fs/*.o kernel/security/*.o kernel/multitasking/*.o kernel/user/*.o boot/*.bin drivers/*.o cpu/*.o libc/*.o libc/string/*.o libc/stdio/*.o libc/stdlib/*.o
	rm -rf build/sys
	rm -rf log/*
