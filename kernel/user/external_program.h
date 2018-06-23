#ifndef EXTERNAL_PROGRAM_H
#define EXTERNAL_PROGRAM_H
#include <stdint.h>
typedef void (*call_module_t)(void);
void call_program(void* address);
#endif