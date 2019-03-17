#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys.h>

void ___s();
void ___e();
void init_print(char* str);
void init_printerr(char* str);

void main(int argc, char* argv[]){
  (void)(argc);
  (void)(argv);
  syscall(14, 0, 0, 0);
  init_print("Rhino OS init v0\n");;
  if(syscall(3,0,0,0) != 1){
    init_printerr("PID != 1, This is not the first process aborting\n");
    while(1);
  }

  init_print("Starting TSH\n");

  char *m = "/dev/initrd/tsh";
  uint32_t pid;
  syscall(1,0,0,0);
  if(!syscall(9,(uint32_t)m,(uint32_t)(&pid),0)){
    syscall(2,0,0,0);
    init_printerr("Could not run program \"");
    syscall(15, 4, 0, 0);
    printf(m);
    printf("\"\n");
    while(1);
  }
  syscall(2, 0, 0, 0);
  syscall(4,pid,0,0);

  syscall(14,0,0,0);
  init_printerr("First user program exited press any key to shutdown\n");
  getchar();
  syscall(24,0,0,0);

  while(1);
}


void init_print(char* str){
  syscall(15, 15, 0, 0);
  printf("[");
  syscall(15, 10, 0, 0);
  printf("INIT");
  syscall(15, 15, 0, 0);
  printf("]: ");
  printf(str);
}

void init_printerr(char* str){
  syscall(15, 15, 0, 0);
  printf("[");
  syscall(15, 10, 0, 0);
  printf("INIT");
  syscall(15, 15, 0, 0);
  printf("]: ");
  syscall(15, 4, 0, 0);
  printf(str);
}