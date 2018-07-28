#ifndef RHINO_LIBC_STDIO_H
#define RHINO_LIBC_STDIO_H
#include "cdefs.h"
#include <stdint.h>
#include <stddef.h>
uint8_t putchar(uint8_t c);
void printf(char* msg);
char getchar();
char* getline(char *lineptr, size_t bufsize);

void clear_screen();


#endif