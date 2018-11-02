#pragma once

#include <rhino/common.h>

#define AVX_YMM_SIZE 32
#define AVX512_ZMM_SIZE 64

extern bool detect_avx();
extern bool init_avx();
extern bool detect_avx_512();