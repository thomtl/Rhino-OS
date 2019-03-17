#pragma once

#include <rhino/common.h>
#include <rhino/arch/x86/isr.h>
#include <rhino/multitasking/scheduler.h>
#include <rhino/arch/x86/drivers/pit.h>
#include <rhino/arch/x86/drivers/hpet.h>

#include <rhino/arch/x86/drivers/rtc.h>

void init_timer();
uint32_t get_tick();
void sleep(uint32_t nSeconds);
void msleep(uint32_t nMseconds);
time_t now();