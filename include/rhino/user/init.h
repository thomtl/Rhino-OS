#pragma once

#include <stdint.h>
#include <rhino/common.h>
#include <rhino/mm/vmm.h>
#include <rhino/mm/pmm.h>
#include <rhino/multitasking/task.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <rhino/user/program.h>
#include <libk/string.h>

#define PROGRAM_LOAD_ADDRESS 0x1000

void init(char *prg);
uint32_t create_process(char* prg, uint32_t* pid);
