#pragma once

#include <rhino/common.h>
#include <rhino/multitasking/task.h>

void schedule(registers_t* regs);
void enable_scheduling();
void disable_scheduling();

