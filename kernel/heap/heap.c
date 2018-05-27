#include "heap.h"
static uint8_t k_heapBMGetNID(uint8_t a, uint8_t b);
void k_heapBMInit(KHEAPBM *heap){
  heap->fblock = 0;
}
int k_heapBMAddBlock(KHEAPBM *heap, uintptr_t addr, uint32_t size, uint32_t bsize){
  KHEAPBLOCKBM *b;
  uint32_t bcnt;
  uint32_t x;
  uint8_t *bm;

  b = (KHEAPBLOCKBM*)addr;
  b->size = size - sizeof(KHEAPBLOCKBM);
  b->bsize = bsize;

  b->next = heap->fblock;
  heap->fblock = b;

  bcnt = b->size / b->bsize;
  bm = (uint8_t*)&b[1];

  for(x = 0; x < bcnt; ++x){
    bm[x] = 0;
  }

  bcnt = (bcnt / bsize) * bsize < bcnt ? bcnt / bsize + 1 : bcnt / bsize;
  for(x = 0; x < bcnt; ++x){
    bm[x] = 5;
  }

  b->lfb = bcnt - 1;
  b->used = bcnt;
  return 1;
}

void* k_heapBMAlloc(KHEAPBM *heap, uint32_t size){
  KHEAPBLOCKBM *b;
  uint8_t *bm;
  uint32_t bcnt, x, y, z, bneed;
  uint8_t nid;

  for(b = heap->fblock; b; b = b->next){
    if(b->size - (b->used * b->bsize) >= size){
      bcnt = b->size / b->bsize;
      bneed = (size / b->bsize) * b->bsize < size ? size / b->bsize + 1 : b->bsize;
      bm = (uint8_t*)&b[1];

      for(x = (b->lfb + 1 >= bcnt ? 0 : b->lfb + 1); x < b->lfb; ++x){
        if(x >= bcnt){
          x  = 0;
        }

        if(bm[x] == 0){
          for(y = 0; bm[x + y] == 0 && y < bneed && (x + y) < bcnt; ++y);
          if(y == bneed){
            nid = k_heapBMGetNID(bm[x - 1], bm[x + y]);

            for(z = 0; z < y; ++z){
              bm[x + z] = nid;
            }

            b->lfb = (x + bneed) - 2;

            b->used += y;

            return (void*)(x * b->size + (uintptr_t)&b[1]);
          }

          x += (y - 1);
          continue;
        }
      }
    }
  }
  return 0;
}
static uint8_t k_heapBMGetNID(uint8_t a, uint8_t b){
  uint8_t c;
  for(c = a + 1; c == b || c == 0; ++c);
  return c;
}
void k_heapBMFree(KHEAPBM *heap, void* ptr){
  KHEAPBLOCKBM *b;
  uintptr_t ptroff;
  uint32_t bi, x, max;
  uint8_t *bm;
  uint8_t id;

  for(b = heap->fblock; b; b = b->next){
    if((uintptr_t)ptr > (uintptr_t)b && (uintptr_t)ptr < (uintptr_t)b + sizeof(KHEAPBLOCKBM) + b->size){
      ptroff = (uintptr_t)ptr - (uintptr_t)&b[1];

      bi = ptroff / b->size;

      bm = (uint8_t*)&b[1];

      id = bm[bi];
      max = b->size / b->bsize;
      for(x = bi; bm[x] == id && x < max; ++x){
        bm[x] = 0;
      }

      b->used -= x - bi;
      return;
    }
  }
  return;
}
