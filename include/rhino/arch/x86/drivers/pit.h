#pragma once
#include <rhino/common.h>
#include <rhino/arch/x86/io.h>

#define PIT_DIVISOR 1193189

#define PIT_CMD 0x43
#define PIT_DAT_CH_0 0x40
#define PIT_DAT_CH_1 0x41
#define PIT_DAT_CH_2 0x42

void init_pit(uint32_t frq);