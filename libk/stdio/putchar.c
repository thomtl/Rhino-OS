#include "../include/stdio.h"

#include "../../kernel/arch/x86/drivers/screen.h"

int putchar(int ic){
  char c = (char) ic;
  char d[2] = {c, '\0'};
  kprint(d);
  return ic;
}
