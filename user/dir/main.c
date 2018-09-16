#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
void start();
void eexit();

void main(void){
  start();
  bool color = false;
  int argc = syscall(0, 12, syscall(0, 7, 0, 0), 0);

  char** argv = (char**)syscall(0, 10, syscall(0, 7, 0, 0), 0);

  if(argc > 2){
    printf("read: to many arguments use -h or --help for help\n");
    eexit();
  }

  if(strcmp(argv[1], "--color") == 0){
    color = true;
  }

  if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
    printf("-----------------------------------------------\n");
    printf("  dir: lists files\n");
    printf("    type --color to use colors\n");
    printf("    or type -h or --help for this screen\n");
    printf("-----------------------------------------------\n");
    eexit();
  }

  int i = 0;
  FILE *node = 0;
  while ( (node = fread_dir(i)) != 0)
  {
    if(color){
      if ((node->flags&0x7) == RHINO_LIBC_FS_DIRECTORY) syscall(1, 3, 1, 0);
      else syscall(1, 3, 2, 0);
    }
    printf(node->name);
    if(color) syscall(1, 3, 15, 0);
    printf("  ");
    i++;
  }
  printf("\n");
  eexit();
}

void start(){
  asm("sti");
  syscall(1, 3, 15, 0);
}

void eexit(){
  syscall(0, 14, syscall(0, 7, 0, 0), 0);
  while(1);
}