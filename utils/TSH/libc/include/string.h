#ifndef RHINO_LIBC_STRING_H
#define RHINO_LIBC_STRING_H
#include "cdefs.h"
#include <stdint.h>
#include <stddef.h>

int strcmp(char s1[], char s2[]);

char * strcpy(char *strDest, char *strSrc);

size_t strlen(const char* str);

void reverse(char s[]);

void int_to_ascii(int n, char str[]);

#endif