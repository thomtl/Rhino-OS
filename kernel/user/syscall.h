#pragma once

#include <stdint.h>
#include "../arch/x86/isr.h"
void syscall_handler(registers_t *regs);
void init_syscall();