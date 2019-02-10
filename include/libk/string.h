#pragma once

#include <libk/sys/cdefs.h>
#include <stdint.h>
#include <stddef.h>

#include <libk/stdlib.h>

void* memcpy(void* __restrict, const void* __restrict, size_t);
int memcmp(const void*, const void*, size_t);
void* memset(void*, int, size_t);
void* memmove(void*, const void*, size_t);
int memcmp(const void*, const void*, size_t);
size_t strlen(const char*);
char *strcpy(char *strDest, char *strSrc);

void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
void reverse(char s[]);
void backspace(char s[]);
void append(char s[], char n);
int strcmp(char s1[], char s2[]);

char* strdup(char* s);