#include "../include/stdlib.h"
#include "../include/string.h"
typedef struct {
  uint32_t* start;
  uint32_t* end;
  uint32_t* nextBlock;
  uint32_t size;
} memoryBlockHeader;

memoryBlockHeader* findLastHeader();


uint32_t memoryBlockHeaderSize = sizeof(memoryBlockHeader);
const uint32_t* memoryAddress = (uint32_t*) 0x7ffff;

void* malloc(size_t size){
  uint32_t totalBlockSize = memoryBlockHeaderSize + size;
  memoryBlockHeader* lastHeader = findLastHeader();
  uint32_t* lastBlock = (uint32_t*)(lastHeader + ((*lastHeader).size));
  memoryBlockHeader block;
  block.start = (uint32_t*)((*lastHeader).end);
  block.size = totalBlockSize;
  block.end = (uint32_t*)(block.start + block.size);
  uint32_t* valueAddress = (uint32_t*)(block.start + memoryBlockHeaderSize);
  uint32_t* headerAddress = lastBlock;
  /*memcpy((headerAddress), (&memoryBlockHeader), (sizeof(memoryBlockHeader)));*/
  memcpy((headerAddress), (&block), (sizeof(memoryBlockHeader)));
  return valueAddress;
}

memoryBlockHeader* findLastHeader(){ // make it found or something
  memoryBlockHeader* localAddress = (memoryBlockHeader*) 0x7ffff;
  uint8_t isFound = 0;
  while(isFound == 0){
    if((memoryBlockHeader*)((*localAddress).nextBlock) == NULL){
        localAddress += (uint32_t)((*localAddress).size);
    } else {
      return (memoryBlockHeader*)localAddress;
    }
  }
  return (memoryBlockHeader*)NULL;
}
