#include <stdint.h>
#include <stdio.h>
#include <sys.h>

void ___s();
void ___e();
void init_print(char* str);
void init_printerr(char* str);

void main(void){
  ___s();
  syscall(1, 2, 0, 0);
  init_print("Rhino OS init v0\n");;
  if(syscall(0,7,0,0) != 1){
    init_printerr("PID != 1, This is not the first process aborting\n");
    while(1);
  }

  init_print("Starting TSH\n");

  char *m = "tsh";
  uint32_t pid;
  syscall(0,2,0,0);
  if(!syscall(0,13,(uint32_t)m,(uint32_t)(&pid))){
    syscall(0,3,0,0);
    init_printerr("Could not run program \"");
    syscall(1, 3, 4, 0);
    printf(m);
    printf("\"\n");
    while(1);
  }
  syscall(0,3,0,0);
  syscall(0,8,pid,0);
  
  while(1);

  ___e();
}


void init_print(char* str){
  syscall(1, 3, 15, 0);
  printf("[");
  syscall(1, 3, 10, 0);
  printf("INIT");
  syscall(1, 3, 15, 0);
  printf("]: ");
  printf(str);
}

void init_printerr(char* str){
  syscall(1, 3, 15, 0);
  printf("[");
  syscall(1, 3, 10, 0);
  printf("INIT");
  syscall(1, 3, 15, 0);
  printf("]: ");
  syscall(1, 3, 4, 0);
  printf(str);
}

void ___s(){
  syscall(0,3,0,0);
  syscall(1, 3, 15, 0);
}

void ___e(){
  uint32_t j = syscall(0,7,0,0);
  syscall(0,14,j,0);
  while(1);
}