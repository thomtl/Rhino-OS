#include <libk/stdlib.h>
#include <rhino/panic.h>
__attribute__((__noreturn__))
void abort(void){
  panic();
  __builtin_unreachable();
}
