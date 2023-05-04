#include "main.h"
#include "my_zsh.h"

int main(int ac, char **av, char **env) {
  if (ac <= 1 && av[0] != NULL)
    my_zsh(env);
  else
    printf("Error: incorrect use. Please read README.md");
  return 0;
}