#include "../include/stdlib.h"

typedef struct {
  uint32_t* start;
  uint32_t* end;
  uint32_t* nextBlock;
  uint32_t size;
} memoryBlockHeader;

uint32_t* findLastHeader();


const uint32_t memoryBlockHeaderSize = sizeof(memoryBlockHeader);
uint32_t* memoryAddress = (uint32_t*) 0x7ffff;

void* malloc(size_t size){
  uint32_t totalBlockSize = size + memoryBlockHeaderSize;
  memoryBlockHeader block;
  block.start = (uint32_t*)(memoryAddress + totalBlockSize);
  block.size = totalBlockSize;
  block.end = (uint32_t*)(block.start + block.size);
  uint32_t* valueAddress = (uint32_t*)(block.start - memoryBlockHeaderSize);
  //memoryAddress += totalBlockSize; not good
  return valueAddress;
}

uint32_t* findLastHeader(){
  uint32_t* localAddress = memoryAddress;
  uint8_t isFound = 0;
  while(isFound == 0){
    if(*localAddress.nextBlock == NULL){
        localAddress += *localAddress.size;
    } else {
      return localAddress;
    }
  }
  return NULL;
}
