#include "include/sys.h"

uint32_t syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx){
  uint32_t ret;
  __asm__ __volatile__ ("int $0x80" : "=a"(ret): "a"(eax), "b"(ebx), "c"(ecx), "d" (edx));
  return ret;
}