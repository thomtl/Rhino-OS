C_SOURCES = $(wildcard src/kernel/*.c src/kernel/mm/*.c src/kernel/types/*.c src/kernel/fs/*.c src/kernel/security/*.c src/kernel/multitasking/*.c src/kernel/pwr/*.c src/kernel/user/*.c src/kernel/arch/x86/drivers/*.c src/kernel/arch/x86/*.c src/libk/*.c src/libk/string/*.c src/libk/stdio/*.c src/libk/stdlib/*.c)
#HEADERS = $(wildcard src/kernel/*.h src/kernel/mm/*.h  src/kernel/types/*.h src/kernel/fs/*.h src/kernel/security/*.h src/kernel/multitasking/*.h src/kernel/power/*.h src/kernel/user/*.h src/kernel/arch/x86/drivers/*.h src/kernel/arch/x86/*.h src/libk/*.h src/libk/include/*.h src/libk/include/sys/*.h)

# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o src/kernel/arch/x86/interrupt.o src/kernel/arch/x86/gdtlow.o src/kernel/multitasking/switch.o src/kernel/mm/mm.o src/kernel/pwr/power_low.o}

# Change this if your cross-compiler is somewhere else
CC = i686-elf-gcc

# -g: Use debugging symbols in gcc
CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=gnu99 -I include/
.PHONY: all clean run bochs initrd
all: run
# First rule is run by default

initrd:
	rm -f initrd.img
	./utils/initrdgen/genrd ./utils/initrdgen/test.txt test.txt ./utils/initrdgen/test1.txt test1.txt ./utils/UserTest/usertest test.prg ./utils/TSH/tsh init.prg

rhino.iso: kernel.bin initrd
	mkdir -p build/sys/boot/grub
	cp kernel.bin build/sys/boot/kernel.bin
	cp initrd.img build/sys/boot/initrd.img
	cp build/grub.cfg build/sys/boot/grub/grub.cfg
	grub-mkrescue -o ./rhino.iso build/sys

kernel.bin: src/kernel/kernel_early.o ${OBJ}
	${CC} -T build/linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

run: rhino.iso
	DISPLAY=:0 qemu-system-x86_64 -m 256M -cdrom rhino.iso -d cpu_reset -D build/log/qemu.log
	rm -rf build/sys

bochs: rhino.iso
	DISPLAY=:0 bochs -f build/.bochsrc -q
	rm -rf build/sys

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c #${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	yasm $< -f elf -o $@ -p nasm
%.o: %.s
	yasm $< -f elf -o $@ -p gas

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf *.iso *.out
	rm -rf src/kernel/*.o src/kernel/mm/*.o src/kernel/types/*.o src/kernel/fs/*.o src/kernel/security/*.o src/kernel/multitasking/*.o src/kernel/pwr/*.o src/kernel/user/*.o src/kernel/arch/x86/drivers/*.o src/kernel/arch/x86/*.o src/libk/*.o src/libk/string/*.o src/libk/stdio/*.o src/libk/stdlib/*.o
	rm -rf build/sys
	rm -rf build/log/*
	rm -f initrd.img
	rm -f snapshot.txt
