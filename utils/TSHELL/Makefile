C_SOURCES = $(wildcard ./*.c libc/*.c)
HEADERS = $(wildcard ./*.h libc/*.h)

# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o}


CC = i686-elf-gcc
LD = i686-elf-ld
CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=c99

tshell: ${OBJ}
	${CC} -T link.ld -o $@ -ffreestanding -nostdlib $^ -lgcc


clean:
	rm -f tshell
	rm -f *.o
	rm -f libc/*.o

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@
