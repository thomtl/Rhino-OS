#include <libk/stdio.h>

#include <rhino/arch/x86/drivers/screen.h>

int putchar(int ic){
  char c = (char) ic;
  char d[2] = {c, '\0'};
  kprint(d);
  return ic;
}
