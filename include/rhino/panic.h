#pragma once

#include <stdint.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <libk/string.h>
__attribute__((noreturn))
void panic();

__attribute__((noreturn))
void panic_m(char* m, char *f);

__attribute__((noreturn))
void panic_assert(char *file, uint32_t line, char *desc);