#ifndef PROGRAM_H
#define PROGRAM_H
#include <stdint.h>

#define PROGRAM_BINARY_TYPE_BIN 0x0
#define PROGRAM_BINARY_TYPE_AOUT 0x1
#define PROGRAM_BINARY_TYPE_ELF 0x2

typedef struct loadedProgram{
    uint32_t* base;
    uint32_t* end;
    uint8_t type;
} loaded_program_t;

typedef void (*call_module_t)(void);

loaded_program_t* load_program(char* filename, uint8_t type);
void free_program(loaded_program_t* header);
void run_program(void *address);

#endif
