#pragma once

#include <stdint.h>
#include "../common.h"
#include "task.h"

#define TICK_TO_SECONDS(a) ((a) / (50))

#define TASK_TIME_SLICE 1

void schedule(uint32_t tick);
void enable_scheduling();
void disable_scheduling();

