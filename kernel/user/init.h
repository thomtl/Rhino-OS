#ifndef INIT_H
#define INIT_H
#include <stdint.h>
#include "../common.h"
#include "../mm/paging.h"
#include "../multitasking/task.h"
#include "../../drivers/screen.h"
#include "program.h"
#include "../../libk/include/string.h"
#define FREE_PROG (MM_PAGE_S * 8 + KHEAP_MAX_SIZE)
#define PROGRAM_LOAD_ADDRESS (void*)0x1000

void init(char *prg);



#endif
