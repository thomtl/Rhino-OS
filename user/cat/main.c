#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys.h>

#include <sys/stat.h>

void main(int argc, char* argv[]){
  if(argc < 2 || argc > 3){
    printf("read: no arguments use -h or --help for help\n");
    return;
  }

  if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
    printf("-----------------------------------------------\n");
    printf("  read: reads a file\n");
    printf("    supply the file name as the first argment\n");
    printf("    or type -h or --help for this screen\n");
    printf("-----------------------------------------------\n");
    return;
  }

  int file = open(argv[1]);
  if(file == -1){
    printf("read: ");
    printf(argv[1]);
    printf(" file doesn't exist\n");
    return;
  }

  struct stat st;

  fstat(file, &st);

  size_t l = st.st_size;

  char *buf = (char*)malloc(l);
  read(buf, l, file);

  for(int32_t i = 0; i < (int64_t)l; i++){
    putchar(buf[i]);
  }

  free(buf);

  close(file);
}