/***************************************************************************//**
  @file         main.c
  @author       Thomas Woertman
  @date         Saturday,  9 June 2018
  @brief        TSHELL
*******************************************************************************/

#include "common.h"

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
int t_execute(char *args);
void t_loop(void);
void main(void)
{
  // Load config files, if any.
  clear_screen();
  // Run command loop.
  t_loop();

  // Perform any shutdown/cleanup.

  exit();
}

char *builtin_str[] = {
  "HELP",
  "EXIT",
  "CLEAR"
};

int (*builtin_func[]) (char *) = {
  &t_help,
  &t_exit,
  &t_clear
};

int t_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
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
  printf("Thomas Woertmans's TSHELL\n");
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
  __asm__("cli");
  clear_screen();
  printf("Goodbye");
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

  return 1;//t_launch(args);
}


/**
   @brief Loop getting input and executing it.
 */
void t_loop(void)
{

  //int status;

  while(1) {
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
