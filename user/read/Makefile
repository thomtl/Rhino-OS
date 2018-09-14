C_SOURCES = $(wildcard ./*.c ../libc/*.c)
HEADERS = $(wildcard ./*.h ../libc/*.h)

OBJ = ${C_SOURCES:.c=.o}

CC = i686-elf-gcc
LD = i686-elf-ld
CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=gnu99 -I../libc/include

read: ${OBJ}
	${CC} -T link.ld -o $@ -ffreestanding -nostdlib $^ -lgcc


clean:
	rm -f read
	rm -f *.o

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@