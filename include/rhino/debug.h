#pragma once

#include <rhino/common.h>
#include <rhino/arch/x86/drivers/serial.h>

#ifdef DEBUG
#define debug_log(m) (debug_log_int(m))
#else
#define debug_log(m) (UNUSED(m))
#endif

void debug_log_int(char* m);