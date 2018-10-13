/***************************************************************************//**
  @file         main.c
  @author       Thomas Woertman
  @date         Saturday,  9 June 2018
  @brief        TSH
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char line[256] = "";

int t_help();
int t_exit();
int t_num_builtins();
int t_help(char **args);
int t_clear(char **args);
int t_exit(char **args);
int t_nothing(char **f);
int t_execute(char **args);
void t_loop(void);

void main(int argc, char* argv[])
{
  (void)(argc);
  (void)(argv);
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

  t_loop();

  exit();
}

char *builtin_str[] = {
  "help",
  "exit",
  "clear",
  ""
};

int (*builtin_func[]) (char **) = {
  &t_help,
  &t_exit,
  &t_clear,
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


int t_nothing(char** args){
  UNUSED(args);
  return 1;
}

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

void t_loop(void)
{
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
    t_execute(t_split_line(line));

  }
}