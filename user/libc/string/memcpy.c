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
    for(uint32_t i = 0; i < size; i += 8){
        asm("movq (%0), %%mm0; movq %%mm0, (%1)" : : "r"((uint32_t)srcptr + i), "r"((uint32_t)dstptr + i) : "memory");
    }
	return dstptr;
}

static inline void* memcpy_movd(void* restrict dstptr, const void* restrict srcptr, size_t size){
    for(uint32_t i = 0; i < size; i += 4){
        asm("movd (%0), %%mm0; movd %%mm0, (%1)" : : "r"((uint32_t)srcptr + i), "r"((uint32_t)dstptr + i) : "memory");
    }
	return dstptr;
}

static inline void* memcpy_movdqu(void* restrict dstptr, const void* restrict srcptr, size_t size){
    for(uint32_t i = 0; i < size; i += SSE_XMM_SIZE){
        asm("movdqu (%0), %%xmm0; movdqu %%xmm0, (%1)" : : "r"((uint32_t)srcptr + i), "r"((uint32_t)dstptr + i) : "memory");
    }
	return dstptr;
}

static inline void* memcpy_movups(void* restrict dstptr, const void* restrict srcptr, size_t size){
    for(uint32_t i = 0; i < size; i += SSE_XMM_SIZE){
        asm("movups (%0), %%xmm0; movups %%xmm0, (%1)" : : "r"((uint32_t)srcptr + i), "r"((uint32_t)dstptr + i) : "memory");
    }
	return dstptr;
}

static inline void* memcpy_movaps(void* restrict dstptr, const void* restrict srcptr, size_t size){
    for(uint32_t i = 0; i < size; i += SSE_XMM_SIZE){
        asm("movaps (%0), %%xmm0; movaps %%xmm0, (%1)" : : "r"((uint32_t)srcptr + i), "r"((uint32_t)dstptr + i) : "memory");
    }
	return dstptr;
}

static inline void* memcpy_vmovaps(void* restrict dstptr, const void* restrict srcptr, size_t size){
	for(uint32_t i = 0; i < size; i += AVX_YMM_SIZE){
        asm("vmovaps (%0), %%ymm0; vmovaps %%ymm0, (%1)" : : "r"((uint32_t)srcptr + i), "r"((uint32_t)dstptr + i) : "memory");
    }
	return dstptr;
}

static inline void* memcpy_vmovups(void* restrict dstptr, const void* restrict srcptr, size_t size){
	for(uint32_t i = 0; i < size; i += AVX_YMM_SIZE){
        asm("vmovups (%0), %%ymm0; vmovups %%ymm0, (%1)" : : "r"((uint32_t)srcptr + i), "r"((uint32_t)dstptr + i) : "memory");
    }
	return dstptr;
}

void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size) {
	if(detect_avx()){
		if(((size % AVX_YMM_SIZE) == 0) && !((size_t)dstptr & (AVX_YMM_SIZE - 1)) && !((size_t)srcptr & (AVX_YMM_SIZE - 1))){
			return memcpy_vmovaps(dstptr, srcptr, size);
		}
		if((size % AVX_YMM_SIZE) == 0){
			return memcpy_vmovups(dstptr, srcptr, size);
		}
	}
	if(detect_sse2()){
		if((size % SSE_XMM_SIZE) == 0){
			return memcpy_movdqu(dstptr, srcptr, size);
		}
	}
	if(detect_sse()){
		if(((size % SSE_XMM_SIZE) == 0) && !((size_t)dstptr & (SSE_XMM_SIZE - 1)) && !((size_t)srcptr & (SSE_XMM_SIZE - 1))){
			return memcpy_movaps(dstptr, srcptr, size);
		}
		if((size % SSE_XMM_SIZE) == 0){
			return memcpy_movups(dstptr, srcptr, size);
		}
	}
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
