#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Function Declarations for builtin shell commands
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

// List of builtin commands, followed by their corresponding functions.
char *builtin_str[] = { "cd", "help", "exit" };
int (*builtin_func[]) (char **) = { &lsh_cd, &lsh_help, &lsh_exit };

// Get the number of builtin commands
int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

// Builtin function implementations

// Builtin command: change directory
int lsh_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

// Builtin command: print help
int lsh_help(char **args) {
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");
  for (i = 0; i < lsh_num_builtins(); i++) {
    printf(" %s\n", builtin_str[i]);
  }
  printf("Use the man command for information on other programs.\n");
  return 1;
}

// Builtin command: exit
int lsh_exit(char **args) {
  return 0;
}

// Launch a program and wait for it to terminate.
int lsh_launch(char **args) {
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

// Execute shell built-in or launch program.
int lsh_execute(char **args) {
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024

// Read a line of input from stdin.
char *lsh_read_line(void) {
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

// Split a line into tokens (very naively).
char **lsh_split_line(char *line) {
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char *));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char *));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

// Function pointer to point display message
typedef void (*pfnMessage)(const char *, float fResult);

// Function pointer to point arithmetic function
typedef float (*pfnCalculator)(float, float);

// Structure of function pointer
typedef struct S_sArithMaticOperation {
  float iResult;
  pfnMessage DisplayMessage;
  pfnCalculator ArithmaticOperation;
} sArithMaticOperation;

// Perform Addition
float Addition(float a, float b) {
  return (a + b);
}

// Perform Subtraction
float Subtraction(float a, float b) {
  return (a - b);
}

// Perform Multiplication
float Multiplication(float a, float b) {
  return (a * b);
}

// Perform Division
float Division(float a, float b) {
  return (a / b);
}

// Function display message
void Message(const char *pcMessage, float fResult) {
  printf("\n\n %s = %f\n\n\n\n", pcMessage, fResult);
}

// Perform arithmetic operation
void PerformCalculation(float x, float y, sArithMaticOperation *funptr, const char *pcMessage) {
  // Call function as per the user choice
  float result = funptr->ArithmaticOperation(x, y);
  // Display the Message
  funptr->DisplayMessage(pcMessage, result);
}

int main(int argc, char **argv) {
  // Load config files, if any.

  // Run command loop.
  char *line;
  char **args;
  int status;
  sArithMaticOperation *pS = NULL;

  pS = malloc(sizeof(sArithMaticOperation));
  if (pS == NULL) {
    return -1;
  }

  pS->DisplayMessage = &Message;

  do {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);

  // Free the allocated memory
  free(pS);
  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
