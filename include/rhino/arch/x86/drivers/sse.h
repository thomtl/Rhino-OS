#pragma once

#include <rhino/common.h>

#define SSE_XMM_SIZE 16

extern bool detect_sse();
extern bool detect_sse2();
extern bool detect_mmx();
extern bool init_sse();