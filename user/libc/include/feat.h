#ifndef RHINO_LIBC_STDLIB_H
#define RHINO_LIBC_STDLIB_H
#include "cdefs.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "sys.h"

#define AVX_YMM_SIZE 32
#define SSE_XMM_SIZE 16

extern bool detect_mmx();
extern bool detect_sse();
extern bool detect_sse2();
extern bool detect_avx();
extern bool detect_avx_512();


#endif
