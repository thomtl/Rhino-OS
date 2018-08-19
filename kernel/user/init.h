#pragma once

#include <stdint.h>
#include "../common.h"
#include <rhino/mm/vmm.h>
#include <rhino/mm/pmm.h>
#include "../multitasking/task.h"
#include "../arch/x86/drivers/screen.h"
#include "program.h"
#include "../../libk/include/string.h"

#define PROGRAM_LOAD_ADDRESS (void*)0x1000

void init(char *prg);
