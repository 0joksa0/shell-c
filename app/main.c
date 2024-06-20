#include <stdio.h>

int main() {
  printf("$ ");
  fflush(stdout);

  // Wait for user input
  char input[100];
  fgets(input, 100, stdin);

  printf("%s: command not found\n", input);

  return 0;
}
