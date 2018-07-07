#ifndef FRAME_H
#define FRAME_H
#include <stdint.h>
#include "../multiboot.h"

#define MMAP_GET_NUM 0
#define MMAP_GET_ADDR 1
#define PAGE_SIZE 4096

typedef struct{
    uint32_t num;
    uint32_t addr;
} frame_t;

void init_frame_alloc(multiboot_info_t *mbd);
frame_t alloc_frame();
#endif