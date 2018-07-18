CC = i686-elf-gcc
LD = i686-elf-ld
CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=c99

tshell:
	yasm start.asm -f elf -o start.o -p nasm
	${CC} ${CFLAGS} -ffreestanding -c main.c -o main.o
	${CC} ${CFLAGS} -ffreestanding -c common.c -o common.o
	${CC} -T link.ld -o $@ -ffreestanding -nostdlib start.o main.o common.o -lgcc
clean:
	rm -f tshell
	rm -f main.o common.o
