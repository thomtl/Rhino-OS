#include "kmalloc.h"
#include "../../libc/include/string.h"

typedef struct {
  uint8_t* start;
  uint8_t* end;
  uint8_t* nextBlock;
  uint8_t size;
} memoryBlockHeader;

memoryBlockHeader* find_last_header();

const uint8_t memoryBlockHeaderSize = sizeof(memoryBlockHeader);

#define MEMORY_ADDRESS 0x7ffff

void* kmalloc(size_t size){
  if(size == 0) return NULL;

  uint8_t totalBlockSize = memoryBlockHeaderSize + (uint8_t)size;

  memoryBlockHeader* lastHeader = find_last_header();
  uint8_t* lastBlock = (*lastHeader).end;
  memoryBlockHeader block;
  block.start = lastBlock + 1;
  block.size = totalBlockSize;
  block.end = (uint8_t*)((block.start) + block.size);
  block.nextBlock = NULL;
  (*lastHeader).nextBlock = block.start;
  memoryBlockHeader* valueAddress = (memoryBlockHeader*)(block.start += memoryBlockHeaderSize);
  uint8_t* headerAddress = lastBlock;
  /*memcpy((headerAddress), (&memoryBlockHeader), (sizeof(memoryBlockHeader)));*/
  memcpy((headerAddress), (&block), (sizeof(block)));
  return (uint32_t*)valueAddress;
}

memoryBlockHeader* find_last_header(){ // make it found or something
  memoryBlockHeader* localAddress = (memoryBlockHeader*) MEMORY_ADDRESS;
  uint8_t isFound = 0;
  while(isFound == 0){
      if((memoryBlockHeader*)((*localAddress).nextBlock) == NULL){
          return (memoryBlockHeader*)(*localAddress).start;
      }
      localAddress += (uint8_t)((*localAddress).size);
  }
  return (memoryBlockHeader*)NULL;
}
void initialize_memory_manager(){
  memoryBlockHeader m;
  m.size = memoryBlockHeaderSize;
  m.start = (uint8_t*)MEMORY_ADDRESS;
  m.end = (uint8_t*)(MEMORY_ADDRESS + memoryBlockHeaderSize);
  m.nextBlock = NULL;
  memcpy((uint8_t*)MEMORY_ADDRESS, &m, sizeof(m));
}
