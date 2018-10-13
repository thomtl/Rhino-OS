#ifndef RHINO_LIBC_MATH_H
#define RHINO_LIBC_STDIO_H
#include "cdefs.h"
#include <stdint.h>
#include "string.h"

#define e 2.718281828459
#define pi 3.14159265358979323846264338327950288
double sqrt(double __x);
double exp(double __x);
double sin(double __x);
double cos(double __x);
double fabs(double __x);
double ceil(double __x);
double pow(double __x, double __y);
#endif