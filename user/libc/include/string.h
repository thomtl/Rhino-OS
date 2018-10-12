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

char *strchr(const char *s, int c);

size_t strcspn(const char *s1, const char *s2);

size_t strspn(const char *s1, const char *s2);

char *strtok(char* str, const char* restrict delim);

void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size);
#endif