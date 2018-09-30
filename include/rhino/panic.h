#pragma once

#include <stdint.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <libk/string.h>
__attribute__((noreturn))
void panic_int(char* file, char* func, uint32_t line);

__attribute__((noreturn))
void panic_m(char* m, char* file, char* func, uint32_t line);

__attribute__((noreturn))
void panic_assert(char *file, char* func, uint32_t line, char *desc);