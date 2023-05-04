#ifndef MY_ZSH_H
#define MY_ZSH_H

#include "main.h"

char *read_line(void);
char **split_line(char *line);
char *which(const char *command);
void my_cd(char **args);
void my_zsh(char **env);
void executor(char **args);
void my_echo(char **args);
void segfault_handler(int sig);
size_t my_unstenv(char** env_copy, char** args, size_t env_size);
size_t my_setenv(char** env_copy, char** args, size_t env_size, size_t env_capacity);
void init_env_copy(char **env, char ***env_copy, size_t *env_size, size_t *env_capacity);

#endif