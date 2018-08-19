#pragma once

#include <stddef.h>
#define EOF (-1)

int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);

char getchar();
char* getline(char lineptr[], size_t bufsize);