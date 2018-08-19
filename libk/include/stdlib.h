#pragma once

#include "sys/cdefs.h"
#include <stddef.h>
#include <stdint.h>
__attribute__ ((__noreturn__))
void abort(void);
void* malloc(size_t size);
void free(void* ptr);
