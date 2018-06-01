C_SOURCES = $(wildcard kernel/*.c kernel/memory/*.c kernel/heap/*.c drivers/*.c cpu/*.c libc/*.c libc/string/*.c libc/stdio/*.c libc/stdlib/*.c)
HEADERS = $(wildcard kernel/*.h kernel/memory/*.h kernel/heap/*.h drivers/*.h cpu/*.h libc/*.h libc/include/*.h libc/include/sys/*.h)

# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o cpu/gdtlow.o}

# Change this if your cross-compiler is somewhere else
CC = i686-elf-gcc
GDB = gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=c99
all: kernel.bin run
# First rule is run by default

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: kernel/kernel_early.o ${OBJ}
	#;i686-elf-ld -T linker.ld -o $@  -Ttext 0x1000 $^ #--oformat binary
	i686-elf-gcc -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc
	mkdir -p build/sys/boot/grub
	cp kernel.bin build/sys/boot/kernel.bin
	cp build/grub.cfg build/sys/boot/grub/grub.cfg
	grub-mkrescue -o ./rhino.iso build/sys --verbose
# Used for debugging purposes
kernel.elf: ${OBJ}
	i686-elf-ld -T linker.ld -o $@ -Ttext 0x1000 $^

run:
	DISPLAY=:0 qemu-system-i386 -cdrom rhino.iso

# Open the connection to qemu and load our kernel-object file with symbols
debug: os-image.bin kernel.elf
	DISPLAY=:0 qemu-system-i386 -s -fda os-image.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f elf -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf *.iso
	rm -rf kernel/*.o kernel/memory/*.o kernel/heap/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o libc/string/*.o libc/stdio/*.o libc/stdlib/*.o bootsect/*.o
	rm -rf build/sys
