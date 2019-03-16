#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>

void main(int argc, char* argv[]){
  if(argc > 1){
    printf("read: to many arguments use -h or --help for help\n");
    return;
  }

  if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
    printf("-----------------------------------------------\n");
    printf("  ls: lists files\n");
    printf("    or type -h or --help for this screen\n");
    printf("-----------------------------------------------\n");
    return;
  }

  int i = 0;
  struct dirent *node = 0;
  while ((node = fread_dir(i)) != 0)
  {
    printf(node->name);
    printf("  ");
    i++;
  }

  printf("\n");
}