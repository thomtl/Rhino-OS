#ifndef RHINO_LIBC_MATH_H
#define RHINO_LIBC_STDIO_H
#include "cdefs.h"
#include <stdint.h>
#include "string.h"

#define e 2.718281828459
#define pi 3.141592653589793
double sqrt(double __x);
float exp(float __x);
float sin(float __x);
float cos(float __x);
float fabs(float __x);
double ceil(double __x);
#endif