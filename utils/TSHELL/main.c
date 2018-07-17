/***************************************************************************//**
  @file         main.c
  @author       Thomas Woertman
  @date         Saturday,  9 June 2018
  @brief        TSHELL
*******************************************************************************/

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
  Function Declarations for builtin shell commands:
 */
int t_cd(char **args);
int t_help(char **args);
int t_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &t_help,
  &t_exit
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
int t_help(char **args)
{
  int i;
  printf("Thomas Woertmans's TSHELL\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < t_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int t_exit(char **args)
{
  return 0;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int t_launch(char **args)
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
}

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
      return (*builtin_func[i])();
    }
  }

  return t_launch(args);
}

#define t_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *t_read_line(void)
{
  int bufsize = t_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    printf("TSHELL: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += t_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        printf("TSHELL: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

/**
   @brief Loop getting input and executing it.
 */
void t_loop(void)
{
  char *line;
  int status;
  char path[1024];
  do {
    printf("> ");
    line = t_read_line();
    status = t_execute(line);

    free(line);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  t_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
