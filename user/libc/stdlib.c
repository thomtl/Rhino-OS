#include "include/stdlib.h"

void* malloc(size_t sz){
    return (void*)syscall(2, 1, sz, 0);
}
void free(void* addr){
    syscall(2, 2, (uint32_t)addr, 0);
}

void exit(){
    syscall(0, 1, 0, 0);
    syscall(0, 2, 0, 0);
}

char* dtoa(double x, char* p){
    uint64_t n;
    uint32_t i = 0, k = 0;
    n = (uint64_t)x;
    while(n>0){
        x /= 10;
        n = (uint64_t)x;
        i++;
    }
    *(p+i) = '.';
    x *= 10;
    n = (uint64_t)x;
    x = x - n;
    while((n>0)||(i>k)){
        if(k == i) k++;
        *(p+k) = '0'+n;
        x *= 10;
        n = (uint64_t)x;
        x = x - n;
        k++;
    }
    *(p+k) = '\0';
    return p;
}

char* itoa(int32_t value, char* str, uint32_t base){
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}