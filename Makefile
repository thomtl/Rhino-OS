C_SOURCES = $(wildcard kernel/*.c kernel/memory/*.c drivers/*.c cpu/*.c libc/*.c libc/string/*.c libc/stdio/*.c libc/stdlib/*.c)
HEADERS = $(wildcard kernel/*.h kernel/memory/*.h drivers/*.h cpu/*.h libc/*.h libc/include/*.h libc/include/sys/*.h)

# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o }

# Change this if your cross-compiler is somewhere else
CC = i686-elf-gcc
GDB = gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=c99
all: os-image.bin run
# First rule is run by default
os-image.bin: bootsect.bin kernel.bin
	cat $^ > os-image.bin

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: bootsect/kernel_entry.o ${OBJ}
	i686-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

# Used for debugging purposes
kernel.elf: bootsect/kernel_entry.o ${OBJ}
	i686-elf-ld -o $@ -Ttext 0x1000 $^

run: os-image.bin
	DISPLAY=:0 qemu-system-i386 -fda os-image.bin

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
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o kernel/memory/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o libc/string/*.o libc/stdio/*.o libc/stdlib/*.o bootsect/*.o
