CC = i686-elf-gcc
LD = i686-elf-ld
CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=c99

usertest:
	${CC} ${CFLAGS} -ffreestanding -c main.c -o main.o
	${LD} -o $@ main.o --oformat binary
clean:
	rm -f usertest
	rm -f main.o
