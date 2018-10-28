#include "../include/string.h"
#include "../include/feat.h"

static inline void* memcpy_movsb(void* restrict dstptr, const void* restrict srcptr, size_t size){
	asm("rep movsb" : : "S"(srcptr), "D"(dstptr), "c"(size) : "memory");
	return dstptr;
}

static inline void* memcpy_movsw(void* restrict dstptr, const void* restrict srcptr, size_t size){
	asm("rep movsw" : : "S"(srcptr), "D"(dstptr), "c"(size / 2) : "memory");
	return dstptr;
}

static inline void* memcpy_movsd(void* restrict dstptr, const void* restrict srcptr, size_t size){
	asm("rep movsd" : : "S"(srcptr), "D"(dstptr), "c"(size / 4) : "memory");
	return dstptr;
}

static inline void* memcpy_movq(void* restrict dstptr, const void* restrict srcptr, size_t size){
	//asm("rep movsd" : : "S"(srcptr), "D"(dstptr), "c"(size) : "memory");
    for(uint32_t i = 0; i < size; i += 8){
        asm("movq (%0), %%mm0; movq %%mm0, (%1)" : : "r"((uint32_t)srcptr + i), "r"((uint32_t)dstptr + i) : "memory");
    }
	return dstptr;
}

static inline void* memcpy_movd(void* restrict dstptr, const void* restrict srcptr, size_t size){
	//asm("rep movsd" : : "S"(srcptr), "D"(dstptr), "c"(size) : "memory");
    for(uint32_t i = 0; i < size; i += 4){
        asm("movd (%0), %%mm0; movd %%mm0, (%1)" : : "r"((uint32_t)srcptr + i), "r"((uint32_t)dstptr + i) : "memory");
    }
	return dstptr;
}



void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size) {
    if(detect_mmx()){
        if((size % 8) == 0){
            return memcpy_movq(dstptr, srcptr, size);
        } else if ((size % 4) == 0){
			return memcpy_movd(dstptr, srcptr, size);
		}
    }
	if((size % 4) == 0){
	    return memcpy_movsd(dstptr, srcptr, size);
	} else if((size % 2) == 0){
	    return memcpy_movsw(dstptr, srcptr, size);
	} else {
	    return memcpy_movsb(dstptr, srcptr, size);
	}
}
