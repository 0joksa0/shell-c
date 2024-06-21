#include <dirent.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT 96082464191
#define ECHO 96082375396
#define TYPE 96086588231

unsigned long hash(const char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash * 33) + c;

  return hash;
}

void typeFunction(char *str, char **envPaths) {
  unsigned long hashCode = hash(str);
  if (hashCode == TYPE || hashCode == EXIT || hashCode == ECHO) {
    printf("%s is a shell builtin\n", str);
    return;
  }
  char **pom = envPaths;
  while (*pom) {
    DIR *d;
    struct dirent *dir;
    d = opendir(*pom);
    if (!d)
      continue;
    while ((dir = readdir(d)) != NULL) {
      if (strstr(dir->d_name, str) != NULL) {
        printf("%s is %s", str, dir->d_name);
        return;
      }
    }
  }
  printf("%s: not found", str);
}

char **getEnvPaths() {
  char *envString = getenv("PATH");
  char **list = NULL;

  int size = 1;
  for (int i = 0; i < strlen(envString); i++) {
    size += envString[i] == ':' ? 1 : 0;
  }

  char *p = strtok(envString, ":");
  list = (char **)malloc(size * sizeof(char *));

  while (p != NULL) {
    list[size - 1] = malloc(20 * sizeof(char));
    strcpy(list[size - 1], p);
    p = strtok(NULL, ":");
    size--;
  }

  return list;
}

int main() {

  // Wait for user input
  char input[100];
  char **envPaths = getEnvPaths();
  char **pom = envPaths;

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
    case TYPE:
      typeFunction(reminder, envPaths);
      break;
    default:
      printf("%s: command not found\n", operation);
      // printf("%lu", hash(input));
    }

  } while (1);

  return 0;
}
