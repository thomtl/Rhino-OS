#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys.h>

void start();
void eexit();

void main(void){
  start();

  int argc = syscall(0, 12, syscall(0, 7, 0, 0), 0);

  char** argv = (char**)syscall(0, 10, syscall(0, 7, 0, 0), 0);

  if(argc < 2 || argc > 3){
    printf("read: no arguments use -h or --help for help\n");
    eexit();
  }

  if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
    printf("-----------------------------------------------\n");
    printf("  read: reads a file\n");
    printf("    supply the file name as the first argment\n");
    printf("    or type -h or --help for this screen\n");
    printf("-----------------------------------------------\n");
    eexit();
  }

  FILE* file = fopen(argv[1]);
  if(file == 0){
    printf("read: ");
    printf(argv[1]);
    printf(" file doesn't exist\n");
    eexit();
  }
  char *buf = (char*)malloc(file->length);
  fread(buf, file->length, file);

  for(uint32_t i = 0; i < file->length; i++){
    char b[2] = {buf[i], '\0'};
    printf(b);
  }
  free(buf);

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