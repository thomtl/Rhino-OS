#pragma once

#include <stdint.h>
#include <rhino/common.h>
#include <rhino/multitasking/task.h>

#define TICK_TO_SECONDS(a) ((a) / (50))

#define TASK_TIME_SLICE 1

void schedule(uint32_t tick, registers_t* regs);
void enable_scheduling();
void disable_scheduling();

