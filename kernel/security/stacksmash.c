#include "stacksmash.h"

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;
__attribute__((noreturn))
void __stack_chk_fail(void){
  PANIC_M("Stack smashing detected!\nPossible intrusion of security!");
}
