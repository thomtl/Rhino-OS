#pragma once

#include <rhino/common.h>
#include <rhino/mm/vmm.h>
#include <rhino/mm/pmm.h>
#include <rhino/multitasking/task.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <rhino/user/program.h>
#include <libk/string.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <rhino/fs/vfs.h>
#include <rhino/fs/initrd.h>
#include <rhino/mm/hmm.h>
#include <rhino/common.h>
#include <libk/string.h>
#include <rhino/multitasking/task.h>
#include <rhino/user/elf.h>

#define PROGRAM_TYPE_UNKOWN 0
#define PROGRAM_TYPE_ELF 1

#define PROGRAM_LOAD_ADDRESS 0x1000

typedef struct loadedProgram{
    uint32_t* base;
    uint32_t* end;
    uint32_t len;
    uint8_t type;
} loaded_program_t;

loaded_program_t* load_program(char* filename);
void free_program(loaded_program_t* header);
void init(char *prg);
uint32_t create_process(char* prg, uint32_t* pid);