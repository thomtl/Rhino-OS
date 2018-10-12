C_SOURCES = $(wildcard ./*.c ../libc/*.c)
HEADERS = $(wildcard ./*.h ../libc/*.h)

ASM_SOURCES = $(wildcard ./*.asm ../libc/*.asm)



C_OBJ = ${C_SOURCES:.c=.o}
ASM_OBJ = ${ASM_SOURCES:.asm=.o}

CC = i686-elf-gcc
LD = i686-elf-ld
AS = yasm

CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=gnu99 -I../libc/include

read: ${C_OBJ} ${ASM_OBJ}
	${CC} -T link.ld -o $@ -ffreestanding -nostdlib $^ -lgcc


clean:
	rm -f tsh
	rm -f *.o

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	${AS} $< -f elf -o $@ -p nasm