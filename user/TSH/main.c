/***************************************************************************//**
  @file         main.c
  @author       Thomas Woertman
  @date         Saturday,  9 June 2018
  @brief        TSH
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
  Function Declarations for builtin shell commands:
 */
/*int t_cd(char **args);
int t_help(char **args);
int t_exit(char **args);*/

/*
  List of builtin commands, followed by their corresponding functions.
 */
 char line[256] = "";
int t_help();
int t_exit();
int t_num_builtins();
int t_help(char *args);
int t_clear(char *args);
int t_exit(char *args);
int t_color(char *args);
int t_read(char *f);
int t_nothing(char *f);
int t_execute(char *args);
void t_loop(void);
void main(void)
{
  // Load config files, if any.
  asm("xchgw %bx, %bx");;
  clear_screen();
  syscall(1, 3, 9, 0);
  printf("Starting TSH v0.0.1, Thomas Woertman 2018\n");
  syscall(1, 3, 4, 0);
  printf("            All Rights Reserved\n");
  syscall(1, 3, 15, 0);
  char buf[25] = "";
  int_to_ascii(syscall(0,7,0,0), buf);
  printf("PID: ");
  printf(buf);
  printf("\n");
  t_color("col");
  //printf("\n");
  // Run command loop.
  t_loop();

  // Perform any shutdown/cleanup.

  exit();
}

char *builtin_str[] = {
  "help",
  "exit",
  "clear",
  "color",
  ""
};

int (*builtin_func[]) (char *) = {
  &t_help,
  &t_exit,
  &t_clear,
  &t_color,
  &t_nothing
};

int t_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}
int t_launch(char* args){
  char* m = malloc(sizeof(char) * strlen(args));
  strcpy(m, args);
  uint32_t pid;
  if(!syscall(2,3,(uint32_t)m,(uint32_t)(&pid))){
    syscall(1, 3, 4, 0);
    printf("[TSH] Could not run program \"");
    printf(args);
    printf("\"\n");
    syscall(1, 3, 15, 0);
    return 0;
  }
  syscall(0,8,pid,0);

  //free(m);

  /**/

  return 1;
}

int t_color(char *args){
  for(uint8_t i = 0; i < 16; i++){
    syscall(1, 3, 0 , i);
    printf(" ");
  }
  printf("\n");
  UNUSED(args);

  return 1;
}

int t_nothing(char *args){
  UNUSED(args);
  return 1;
}

/*
  Builtin function implementations.
*/

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int t_help(char *args)
{
  int i;
  printf("Thomas Woertmans's TSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < t_num_builtins(); i++) {
    printf(builtin_str[i]);
    printf("\n");
  }

  printf("Use the man command for information on other programs.\n");
  UNUSED(args);
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int t_exit(char *args)
{
  __asm__ ("cli");
  clear_screen();
  syscall(1, 3, 14, 0);
  for(int i = 0; i < 980; i++) printf(" ");
  printf("It is now safe to turn of your pc");
  exit();
  while(1);
  UNUSED(args);
  return 1;
}


int t_clear(char *args){
  clear_screen();
  UNUSED(args);
  return 1;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
/*int t_launch(char **args)
{
  pid_t pid;
  pid_t current_pid;
  int status;
  current_pid = get_current_pid();
  pid = fork();
  if (pid != current_pid) {
    // Child process
    if (execvp(args[0], args) == -1) {
      printf("TSHELL: Child Process not started successfully!");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    printf("TSHELL: Forking Error");
  } else if (pid == current_pid){
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}*/

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int t_execute(char *args)
{
  int i;

  for (i = 0; i < t_num_builtins(); i++) {
    if (strcmp(args, builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return t_launch(args);
}


/**
   @brief Loop getting input and executing it.
 */
void t_loop(void)
{

  //int status;

  while(1) {
    syscall(1, 3, 2, 0);
    printf("admin");
    syscall(1, 3, 15, 0);
    printf("@");
    syscall(1, 3, 1, 0);
    printf("localhost  ");
    syscall(1, 3, 15, 0);
    printf("> ");
    getline(line, 256);
  //  printf(line);
    t_execute(line);

    //free(line);
  }
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
