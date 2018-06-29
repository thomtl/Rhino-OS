#include "test_security.h"
#include "../../libc/include/string.h"
void do_stacksmash(char *bar){
  char  c[12];

  strcpy(c, bar);
}
