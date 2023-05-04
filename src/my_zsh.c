#include <stdio.h>
#include <unistd.h>
#include "main.h"

// char *read_line(void);
// char **split_line(char *line);
// char *which(const char *command);
// void my_cd(char **args);
// void my_zsh(char **env);
// void executor(char **args);
// void my_echo(char **args);
// void segfault_handler(int sig);
// size_t my_unstenv(char** env_copy, char** args, size_t env_size);
// size_t my_setenv(char** env_copy, char** args, size_t env_size, size_t env_capacity);
// void init_env_copy(char **env, char ***env_copy, size_t *env_size, size_t *env_capacity);

void my_zsh(char **env) {
  char *line;
  char **args, **env_copy;
  size_t env_size, env_capacity;
  init_env_copy(env, &env_copy, &env_size, &env_capacity);
  signal(SIGSEGV, segfault_handler);
  do {
    printf("ZSH $> ");
    fflush(stdout);
    line = read_line();
    args = split_line(line);
    if (args[0] == NULL)
        printf("Please enter a command\n");
    else if (strcmp(args[0], "cd") == 0)
      my_cd(args);
    else if (strcmp(args[0], "exit") == 0)
      exit(0);
    else if (strcmp(args[0], "echo") == 0)
      my_echo(args);
    else if (strcmp(args[0], "pwd") == 0)   {
      char buffer[BUFFER_SIZE];
      char *pwd = getcwd(buffer, sizeof(buffer));
      printf("%s\n", pwd);
    }
    else if (strcmp(args[0], "env") == 0)  {
      for (size_t i = 0; i < env_size; i++)
        printf("%s\n", env_copy[i]);
    }
    else if (strcmp(args[0], "setenv") == 0) 
      env_size = my_setenv(env_copy, args, env_size, env_capacity);
    else if (strcmp(args[0], "unsetenv") == 0) 
      env_size = my_unstenv(env_copy, args, env_size);
    else if (strcmp(args[0], "which") == 0) {
      char *path_command = which(args[1]);
      if (path_command != NULL)
        printf("%s\n", path_command);
      else
        printf("Command %s not found\n", args[1]);
      free(path_command);
    }
    else
      executor(args);
    free(line);
    free(args);
  } while (1);
  free(env_copy);
}

size_t my_unstenv(char** env_copy, char** args, size_t env_size) {
  char *name = args[1];
  unsetenv(name);
  size_t new_size = 0;
  for (size_t i = 0; i < env_size; i++) {
    if (strstr(env_copy[i], name) != env_copy[i]) {
      env_copy[new_size++] = env_copy[i];
    } else {
      free(env_copy[i]);
    }
  }
  env_copy = (char **)realloc(env_copy, sizeof(char *) * (new_size + 1));
  env_copy[new_size] = NULL;
  return new_size;
}

size_t my_setenv(char** env_copy, char** args, size_t env_size, size_t env_capacity) {
    if (args[1] == NULL || args[2] == NULL) {
        printf("Not enough arguments for setenv\n");
        return 0;
    }
    char* name = args[1];
    char* value = args[2];
    setenv(name, value, 1);
    for (size_t i = 0; i < env_size; i++) {
        if (strncmp(env_copy[i], name, strlen(name)) == 0) {
            free(env_copy[i]);
            env_copy[i] = (char*)malloc(strlen(name) + strlen(value) + 2);
            strcpy(env_copy[i], name);
            strcat(env_copy[i], "=");
            strcat(env_copy[i], value);
            return env_size;
        }
    }
    if (env_size == env_capacity) {
        env_capacity *= 2;
        env_copy = (char**)realloc(env_copy, sizeof(char*) * (env_capacity));
    }
    env_copy[env_size++] = (char*)malloc(strlen(name) + strlen(value) + 2);
    strcpy((env_copy)[env_size-1], name);
    strcat((env_copy)[env_size-1], "=");
    strcat((env_copy)[env_size-1], value);
    return env_size;
}

char *read_line(void) { // to read the input
  char *line = malloc(BUFFER_SIZE);
  fgets(line, BUFFER_SIZE, stdin);
  return line;
}

char **split_line(char *line) { // to split the line after readline into tokens
  char **tokens = malloc(BUFFER_SIZE);
  int i = 0;
  tokens[i] = strtok(line, TOKEN_DELIM);
  while (tokens[i] != NULL)  {
    i++;
    tokens[i] = strtok(NULL, TOKEN_DELIM);
  }
  return tokens;
}

void my_echo(char **args) {
  int i = 1;
  while (args[i] != NULL)  {
    if (args[i][0] == '$')  {
      char *var_name = args[i] + 1; // remove the $ character
      char *var_value = getenv(var_name);
      if (var_value != NULL)
        printf("%s ", var_value);
      else
        printf("(undefined variable %s)", var_name);
    }
    else
      printf("%s ", args[i]);
    i++;
  }
  printf("\n");
}

char *which(const char *command) { // which builtin
    char *path = getenv("PATH");
    path = strdup(path);
    char *dir = strtok(path, ":");
    char *path_command;
    while (dir) {
      path_command = (char *)malloc(strlen(dir) + strlen(command) + 2);
      sprintf(path_command, "%s/%s", dir, command);
      if (access(path_command, X_OK) == 0){
        free(path);
        return path_command;
      }
      dir = strtok(NULL, ":");
      free(path_command);
    }
    free(path);
    return NULL;
}

void my_cd(char **args) { // cd builtin to handle different types of cd
  if (args[1] != NULL)  {
    if (strcmp(args[1], "-") != 0)
    {
      char *cur_dir = getcwd(NULL, 0);
      setenv("OLDPWD", cur_dir, 1);
    }
  }
  if (args[1] == NULL) {
    char *home = getenv("HOME");
    if (home == NULL)
      printf("Error: HOME not set\n");
    if (chdir(home) != 0)
      perror("chdir");
  }
  else if (strcmp(args[1], "-") == 0)  {
    char *prev_dir = getenv("OLDPWD");
    if (prev_dir == NULL)
      printf("Error: OLDPWD not set\n");
    if (chdir(prev_dir) != 0)
      perror("chdir");
  }
  else {
    if (chdir(args[1]) != 0)
      perror("chdir");
  }
}

void segfault_handler(int sig) {
  fprintf(stderr, "Process %d error: signal %d: Segmentation Fault\n", getpid(), sig);
  exit(EXIT_FAILURE);
}

void executor(char **args) {
  int status;
  char *cmd; 
  if (access(args[0], X_OK) == 0)
    cmd = args[0];
  else 
    cmd = which(args[0]);
  pid_t pid = fork();
  if (pid == 0 && cmd != NULL) {
    char *envp = getenv("ENV_VAR");
    if (execve(cmd, args, &envp) < 0) {
      perror("Error executing command");
      exit(EXIT_FAILURE);
    }
  }
  else if (pid > 0) {
    if (waitpid(pid, &status, 0) < 0)
      perror("Error waiting for child process");
    if (WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV) 
      printf("Child process terminated due to segmentation fault\n");
  }
  else  {
    perror("Error forking");
    exit(EXIT_FAILURE);
  }
}

void init_env_copy(char **env, char ***env_copy, size_t *env_size, size_t *env_capacity) {
    *env_size = 0;
    *env_capacity = INITIAL_CAPACITY;
    *env_copy = (char **)malloc(sizeof(char *) * (*env_capacity));
    for (int i = 0; env[i] != NULL; i++)  {
        if (*env_size == *env_capacity)  {
            *env_capacity *= 2;
            *env_copy = (char **)realloc(*env_copy, sizeof(char *) * (*env_capacity));
        }
        (*env_copy)[(*env_size)++] = strdup(env[i]);
    }
}