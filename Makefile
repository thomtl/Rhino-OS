C_SOURCES = $(wildcard src/kernel/*.c src/kernel/mm/*.c src/kernel/Deer/*.c src/kernel/types/*.c src/kernel/fs/*.c src/kernel/security/*.c src/kernel/multitasking/*.c src/kernel/pwr/*.c src/kernel/user/*.c src/kernel/arch/x86/drivers/*.c src/kernel/acpi/*.c src/kernel/arch/x86/*.c src/libk/*.c src/libk/string/*.c src/libk/stdio/*.c src/libk/stdlib/*.c)
ASM_SOURCES = $(wildcard src/kernel/*.asm src/kernel/mm/*.asm src/kernel/Deer/*.asm src/kernel/types/*.asm src/kernel/fs/*.asm src/kernel/security/*.asm src/kernel/multitasking/*.asm src/kernel/pwr/*.asm src/kernel/user/*.asm src/kernel/arch/x86/drivers/*.asm src/kernel/arch/x86/*.asm src/kernel/acpi/*.asm src/libk/*.asm src/libk/string/*.asm src/libk/stdio/*.asm src/libk/stdlib/*.asm src/kernel/*.s src/kernel/mm/*.s src/kernel/Deer/*.s src/kernel/types/*.s src/kernel/fs/*.s src/kernel/security/*.s src/kernel/multitasking/*.s src/kernel/pwr/*.s src/kernel/user/*.s src/kernel/arch/x86/drivers/*.s src/kernel/arch/x86/*.s src/kernel/acpi/*.s src/libk/*.s src/libk/string/*.s src/libk/stdio/*.s src/libk/stdlib/*.s)

C_OBJ = ${C_SOURCES:.c=.o}
ASM_OBJ = ${ASM_SOURCES:.asm=.o}

CC = i686-elf-gcc
AS = yasm
QEMU = qemu-system-x86_64
# -fda flp.flp 
QEMUFLAGS = -m 256M -hda rhino.iso -d cpu_reset -D build/log/qemu.log -rtc base=localtime -monitor stdio -soundhw pcspk -vga std -serial file:/dev/stdout -drive id=disk,file=flp.flp,if=none -device ahci,id=ahci -device ide-drive,drive=disk,bus=ahci.0 

CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=gnu99 -I include/
					
.PHONY: all clean run bochs initrd

all: run

initrd:
	rm -f initrd.img
	./user/makeall.sh
	./utils/initrdgen/genrd ./utils/initrdgen/test.txt test.txt ./utils/initrdgen/test1.txt test1.txt ./user/example/example example ./user/TSH/tsh tsh  ./user/init/init init # ./user/ls/ls ls ./user/cat/cat cat

rhino.iso: kernel.bin initrd
	mkdir -p build/sys/boot/grub
	cp kernel.bin build/sys/boot/kernel.bin
	cp initrd.img build/sys/boot/initrd.img
	cp build/grub.cfg build/sys/boot/grub/grub.cfg
	grub-mkrescue -o ./rhino.iso build/sys

kernel.bin: ${ASM_OBJ} ${C_OBJ}
	${CC} -T build/linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

run: rhino.iso
	rm -rf build/log/*
	DISPLAY=:0 QEMU_AUDIO_DRV=pa ${QEMU} ${QEMUFLAGS}
	rm -rf build/sys

bochs: rhino.iso
	rm -rf build/log/*
	DISPLAY=:0 bochs -f build/.bochsrc -q
	rm -rf build/sys

%.o: %.c
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	${AS} $< -f elf -o $@ -p nasm
%.o: %.s
	${AS} $< -f elf -o $@ -p gas

clean:
	rm -rf *.bin *.o *.iso *.img
	rm -rf src/kernel/*.o src/kernel/mm/*.o src/kernel/Deer/*.o src/kernel/types/*.o src/kernel/fs/*.o src/kernel/security/*.o src/kernel/multitasking/*.o src/kernel/pwr/*.o src/kernel/user/*.o src/kernel/arch/x86/drivers/*.o src/kernel/arch/x86/*.o src/kernel/acpi/*.o src/libk/*.o src/libk/string/*.o src/libk/stdio/*.o src/libk/stdlib/*.o
	rm -rf build/sys
	rm -rf build/log/*
	rm -f snapshot.txt
