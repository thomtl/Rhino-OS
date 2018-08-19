#pragma once

#include <stdint.h>
#include "../common.h"
#include "../mm/vmm.h"
#include "../mm/pmm.h"
#include "../multitasking/task.h"
#include "../../drivers/screen.h"
#include "program.h"
#include "../../libk/include/string.h"

#define PROGRAM_LOAD_ADDRESS (void*)0x1000

void init(char *prg);
