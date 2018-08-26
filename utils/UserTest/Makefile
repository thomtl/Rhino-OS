C_SOURCES = $(wildcard ./*.c)
HEADERS = $(wildcard ./*.h)

# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o}


CC = i686-elf-gcc
LD = i686-elf-ld
CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
					-Wall -Wextra -Werror -std=gnu99

usertest: ${OBJ}
	${CC} -T link.ld -o $@ -ffreestanding -nostdlib $^ -lgcc


clean:
	rm -f usertest
	rm -f *.o

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@
