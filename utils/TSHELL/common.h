#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

uint32_t syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);
void printf(char* m);
void* malloc(size_t sz);
void free(void* addr);
void exit();
char getchar();
int strcmp(char s1[], char s2[]);
char* getline(char lineptr[], size_t bufsize);
void clear_screen();
#define UNUSED(x) (void)(x)
#endif