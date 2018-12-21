#pragma once

#include <rhino/common.h>
void init_timer();
uint32_t get_tick();
void sleep(uint32_t nSeconds);
void msleep(uint32_t nMseconds);