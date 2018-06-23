#ifndef PROGRAM_H
#define PROGRAM_H
#include <stdint.h>

#define PROGRAM_BINARY_TYPE_BIN 0
#define PROGRAM_BINARY_TYPE_AOUT 1
#define PROGRAM_BINARY_TYPE_ELF 2

typedef struct loadedProgram{
    uint32_t* base;
    uint32_t* end;
    uint8_t type;
} loaded_program_t;
// Filename is on the INITRD
loaded_program_t* load_program(char* filename, uint8_t type);

#endif