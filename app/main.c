#include <stdio.h>
#include <string.h>

int main() {

  // Wait for user input
  char input[100];
  do {
    printf("$ ");
    fflush(stdout);

    fgets(input, 100, stdin);
    input[strcspn(input, "\n")] = 0;
    printf("%s: command not found\n", input);
  } while (strcmp(input, "exit") != 0);

  return 0;
}
