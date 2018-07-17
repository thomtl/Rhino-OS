CC = i686-elf-gcc
LD = i686-elf-ld
CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=c99

tshell:
	${CC} ${CFLAGS} -ffreestanding -c main.c -o main.o
	${CC} ${CFLAGS} -ffreestanding -c common.c -o common.o
	${CC} ${CFLAGS} -T link.ld -o $@ main.o common.o
clean:
	rm -f tshell
	rm -f main.o common.o
