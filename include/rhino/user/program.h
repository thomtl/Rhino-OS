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

#define PROGRAM_BINARY_TYPE_BIN 0x0
#define PROGRAM_BINARY_TYPE_AOUT 0x1
#define PROGRAM_BINARY_TYPE_ELF 0x2

#define PROGRAM_LOAD_ADDRESS 0x1000

typedef struct loadedProgram{
    uint32_t* base;
    uint32_t* end;
    uint32_t len;
    uint8_t type;
} loaded_program_t;

typedef void (*call_module_t)(void);

loaded_program_t* load_program(char* filename, uint8_t type);
void free_program(loaded_program_t* header);
void run_program(void *address);
void init(char *prg);
uint32_t create_process(char* prg, uint32_t* pid);