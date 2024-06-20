#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT 405948411209103
#define ECHO 96082375396

unsigned long hash(const char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash * 33) + c;

  return hash;
}

int main() {

  // Wait for user input
  char input[100];
  do {
    printf("$ ");
    fflush(stdout);

    fgets(input, 100, stdin);
    input[strcspn(input, "\n")] = 0;
    char *reminder;
    char *operation = strtok_r(input, " ", &reminder);

    switch (hash(operation)) {

    case EXIT:
      exit(0);
      break;
    case ECHO:
      printf("%s\n", reminder);
      break;
    default:
      printf("%s: command not found\n", operation);
      // printf("%lu", hash(input));
    }

  } while (1);

  return 0;
}
