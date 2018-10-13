#include <math.h>
extern double sqrt_int(double x);
double sqrt(double __x){
    return sqrt_int(__x);
}
extern double sin_int(double x);
extern double fdiv_int(double x, double y);
extern double fmul_int(double x, double y);
double sin(double __x){
    return sin_int(fmul_int(fdiv_int(__x, 180), pi));//sin_int(fdiv_int(fmul_int(__x, pi), 180.0));
}
extern double cos_int(double x);
double cos(double __x){
    return cos_int(fmul_int(fdiv_int(__x, 180), pi));;
}
extern double fabs_int(double x);
double fabs(double __x){
    return fabs_int(__x);
}

double exp(double __x){
    return pow(e, __x);
}

double pow(double __x, double __y){
    double res = __x;
    for(uint32_t i = 0; i < (__y - 1); i++){
        res = fmul_int(res, __x);
    }
    return res;
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