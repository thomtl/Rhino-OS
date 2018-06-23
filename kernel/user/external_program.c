#include "external_program.h"

void call_program(void* address){
    call_module_t start = (call_module_t) address;
    start();
    return;
}