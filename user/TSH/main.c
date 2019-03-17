/***************************************************************************//**
  @file         main.c
  @author       Thomas Woertman
  @date         Saturday,  9 June 2018
  @brief        TSH
*******************************************************************************/

#include <unistd.h>
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
int t_cd(char **args);
int t_exit(char **args);
int t_nothing(char **f);
int t_execute(char **args);
void t_loop(void);

void main(int argc, char* argv[])
{
  (void)(argc);
  (void)(argv);
  clear_screen();
  syscall(15, 9, 0, 0);
  printf("Starting TSH v0.0.1, Thomas Woertman 2018\n");
  syscall(15, 4, 0, 0);
  printf("            All Rights Reserved\n");
  syscall(15, 15, 0, 0);
  char buf[25] = "";
  itoa(syscall(3,0,0,0), buf, 10);
  printf("PID: ");
  printf(buf);
  printf("\n");

  t_loop();

  exit();
}

char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "clear",
  ""
};

int (*builtin_func[]) (char **) = {
  &t_cd,
  &t_help,
  &t_exit,
  &t_clear,
  &t_nothing
};

int t_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}
int t_launch(char** args){
  char* m = malloc(strlen(args[0]) + 1);

  memset(m, 0, strlen(args[0]) + 1);
  memcpy(m, args[0], strlen(args[0]));

  uint32_t pid;
  syscall(1,0,0,0);
  if(!syscall(9,(uint32_t)m,(uint32_t)(&pid),0)){
    syscall(2,0,0,0);
    syscall(15, 4, 0, 0);
    printf("[TSH] Could not run program \"");
    printf(args[0]);
    printf("\"\n");
    syscall(15, 15, 0, 0);
    return 0;
  }

  char buf[128] = "";
  getcwd(buf, 128);

  syscall(11, (uint32_t)buf, pid, 0);


  uint32_t argc = 0; 
  while(args[++argc]);
  
  syscall(7, pid, argc, 0);
  syscall(5, pid, (uint32_t)args, 0);
  syscall(2,0,0,0);
  syscall(4,pid,0,0);

  return 1;
}


int t_nothing(char** args){
  UNUSED(args);
  return 1;
}

int t_cd(char **args){
  if (args[1] == NULL) {
    printf("TSHELL: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      printf("TSHELL: can't cd");
    }
  }
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
  syscall(2,0,0,0);
  clear_screen();
  syscall(15, 14, 0, 0);
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
    syscall(15, 2, 0, 0);
    printf("admin");
    syscall(15, 15, 0, 0);
    printf("@");
    syscall(15, 1, 0, 0);
    printf("localhost:  ");

    syscall(15, 15, 0, 0);

    char buf[128];
    memset(buf, 0, 128);
    getcwd(buf, 128);

    printf(buf);

    printf(" > ");
    getline(line, 256);
    t_execute(t_split_line(line));

  }
}