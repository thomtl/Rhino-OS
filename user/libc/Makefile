C_SOURCES = $(wildcard ./*.c)
HEADERS = $(wildcard ./*.h)

ASM_SOURCES = $(wildcard ./*.asm)

C_OBJ = ${C_SOURCES:.c=.o}
ASM_OBJ = ${ASM_SOURCES:.asm=.o}

CC = i686-elf-gcc
LD = i686-elf-ld
AS = yasm

CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=gnu99 -I../libc/include

all: crt0 libc.o

crt0: crt/crt0.o
	mv crt/crt0.o ./


libc.o: ${ASM_OBJ} ${C_OBJ}
	${CC} ${CFLAGS} -r -o $@ $^


clean:
	rm -f libc.o crt0.o
	rm -f *.o crt/*.o

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	${AS} $< -f elf -o $@ -p nasm