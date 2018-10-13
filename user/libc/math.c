#include <math.h>
extern double sqrt_int(double x);
double sqrt(double __x){
    return sqrt_int(__x);
}
extern float sin_int(float x);
float sin(float __x){
    return sin_int(__x);
}
extern float cos_int(float x);
float cos(float __x){
    return cos_int(__x);
}
extern float fabs_int(float x);
float fabs(float __x){
    return fabs_int(__x);
}
extern float fmul_int(float x, float y);
float exp(float __x){
    float result = e;
    for(uint32_t i = 0; i < __x; i++) result = fmul_int(result, e);
    return result;
}

double ceil(double __x){
    uint64_t input;
    memcpy(&input, &__x, 8);
    int32_t exponent = ((input >> 52) & 0x7FF) - 127;
    if (exponent < 0) return (__x > 0);
    // small numbers get rounded to 0 or 1, depending on their sign

    int32_t fractional_bits = 52 - exponent;
    if (fractional_bits <= 0) return __x;
    // numbers without fractional bits are mapped to themselves

    uint64_t integral_mask = 0xffffffffffffffff << fractional_bits;
    uint64_t output = input & integral_mask;
    // round the number down by masking out the fractional bits

    memcpy(&__x, &output, 8);
    if (__x > 0 && output != input) ++__x;
    // positive numbers need to be rounded up, not down

    return __x;
}