#include <math.h>
extern float sqrt_int(float x);
float sqrt(float __x){
    return sqrt_int(__x);
}
extern float sin_int(float x);
float sin(float __x){
    return sin_int(__x);
}

float ceil(float __x){
    unsigned input;
    memcpy(&input, &__x, 4);
    int exponent = ((input >> 23) & 255) - 127;
    if (exponent < 0) return (__x > 0);
    // small numbers get rounded to 0 or 1, depending on their sign

    int fractional_bits = 23 - exponent;
    if (fractional_bits <= 0) return __x;
    // numbers without fractional bits are mapped to themselves

    unsigned integral_mask = 0xffffffff << fractional_bits;
    unsigned output = input & integral_mask;
    // round the number down by masking out the fractional bits

    memcpy(&__x, &output, 4);
    if (__x > 0 && output != input) ++__x;
    // positive numbers need to be rounded up, not down

    return __x;
}