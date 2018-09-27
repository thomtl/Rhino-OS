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
int t_help(char **args);
int t_clear(char **args);
int t_exit(char **args);
int t_color(char **args);
int t_read(char **f);
int t_nothing(char **f);
int t_execute(char **args);
void t_loop(void);
void main(void)
{
  // Load config files, if any.
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
  t_color(0);

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

int (*builtin_func[]) (char **) = {
  &t_help,
  &t_exit,
  &t_clear,
  &t_color,
  &t_nothing
};

int t_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}
int t_launch(char** args){
  char* m = malloc(sizeof(char) * strlen(args[0]));
  strcpy(m, args[0]);
  uint32_t pid;
  syscall(0,2,0,0);
  if(!syscall(0,13,(uint32_t)m,(uint32_t)(&pid))){
    syscall(0,3,0,0);
    syscall(1, 3, 4, 0);
    printf("[TSH] Could not run program \"");
    printf(args[0]);
    printf("\"\n");
    syscall(1, 3, 15, 0);
    return 0;
  }
  uint32_t argc = 0; 
  while(args[++argc]);
  
  syscall(0, 11, pid, argc);
  syscall(0, 9, pid, (uint32_t)args);
  syscall(0,3,0,0);
  syscall(0,8,pid,0);
  

  return 1;
}

int t_color(char** args){
  for(uint8_t i = 0; i < 16; i++){
    syscall(1, 3, 0 , i);
    printf(" ");
  }
  printf("\n");
  UNUSED(args);

  return 1;
}

int t_nothing(char** args){
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
int t_help(char** args)
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
int t_exit(char** args)
{
  syscall(0,3,0,0);
  clear_screen();
  syscall(1, 3, 14, 0);
  for(int i = 0; i < 980; i++) printf(" ");
  printf("It is now safe to turn of your pc");
  exit();
  while(1);
  UNUSED(args);
  return 1;
}


int t_clear(char** args){
  clear_screen();
  UNUSED(args);
  return 1;
}

#define t_TOK_BUFSIZE 64
#define t_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **t_split_line(char *line)
{
  int bufsize = t_TOK_BUFSIZE, position = 0;
  char *linecop = malloc(strlen(line));
  strcpy(linecop, line);
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    printf("TSH: allocation error\n");
    return 0;
  }

  token = strtok(linecop, t_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      printf("TSH: allocation error\n");
      return 0;
    }

    token = strtok(NULL, t_TOK_DELIM);
  }
  tokens[position] = NULL;

  return tokens;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int t_execute(char** args)
{
  int i;

  for (i = 0; i < t_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
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
    t_execute(t_split_line(line));

    //free(line);
  }
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
