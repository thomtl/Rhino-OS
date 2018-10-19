// x87 FPU driver
#pragma once

#include <rhino/common.h>
#include <rhino/arch/x86/cpuid.h>
#include <rhino/arch/x86/isr.h>
#include <rhino/multitasking/task.h>
extern uint32_t probe_fpu();
extern void enable_native_interrupt_fpu();
extern void init_fpu_low();
extern uint32_t fpu_get_status_word();
bool init_fpu();
void save_fpu_context(task_t* task);
void restore_fpu_context(task_t* task);