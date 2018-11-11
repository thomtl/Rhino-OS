#include <libk/string.h>

int memcmp(const void* aptr, const void* bptr, size_t size){
    const unsigned char *p1 = aptr, *p2 = bptr;
    while(size--)
        if( *p1 != *p2 )
            return *p1 - *p2;
        else
            p1++,p2++;
    return 0;
}
