#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT 96082464191
#define ECHO 96082375396
#define TYPE 96086588231

#define PATH_MAX 4096

int size = 0;

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
  if (envPaths != NULL)
    for (int i = 0; i < size; i++) {
      DIR *d;
      struct dirent *dir;
      // printf("%s", pom[i]);
      d = opendir(pom[i]);
      if (!d) {
        //        closedir(d);
        continue;
      }
      while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, str) == 0) {
          char *result;
          printf("%s is %s/%s\n", str, pom[i], dir->d_name);
          return;
        }
      }
      closedir(d);
    }
  printf("%s: not found\n", str);
  return;
}

void systemOtherFunction(char *operation, char *params, char **envPaths) {

  char **pom = envPaths;
  if (envPaths != NULL)
    for (int i = 0; i < size; i++) {
      DIR *d;
      struct dirent *dir;
      d = opendir(pom[i]);
      if (!d) {
        continue;
      }
      while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, operation) == 0) {
          FILE *fp;

          int status;

          char data[1024];

          char *command = (char *)malloc(sizeof(char) * 100);
          strcpy(command, pom[i]);
          strcat(command, "/");
          strcat(command, operation);
          strcat(command, " ");
          strcat(command, params);
          fp = popen(command, "w");
          if (fp == NULL) {
            printf("Error");
          }

          while (fgets(data, 1024, fp) != NULL)
            printf("console :%s \n", data);

          status = pclose(fp);

          // if (status == -1) {
          // perror("pclose");
          //}
          // else if (WIFSIGNALED(status)) {
          //  printf("terminating signal: %d", WTERMSIG(status));
          // } else if (WIFEXITED(status)) {
          //  printf("exit with status: %d", WEXITSTATUS(status));
          // } else {
          //  printf("unexpected: %d", status);
          // }

          return;
        }
      }
      closedir(d);
    }
  printf("%s: not found\n", operation);
}

char **getEnvPaths() {
  char *envString = getenv("PATH");
  // printf("%s", envString);
  char **list = NULL;

  if (strlen(envString) <= 0) {
    return list;
  }

  size = 1;
  for (int i = 0; i < strlen(envString); i++) {
    size += envString[i] == ':' ? 1 : 0;
  }

  // printf("%d\n", size);

  list = (char **)malloc(size * sizeof(char *));
  char *p = strtok(envString, ":");

  for (int i = 0; i < size; i++) {
    list[i] = malloc((strlen(p) + 2) * sizeof(char));
    strcpy(list[i], p);
    // printf("p: %s list: %s size: %lu\n", p, list[i], strlen(p));
    p = strtok(NULL, ":");
  }

  return list;
}

int main(int argc, char *argv[]) {

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
      systemOtherFunction(operation, reminder, envPaths);
      // printf("%s: command not found\n", operation);
      //  printf("%lu", hash(input));
    }

  } while (1);

  return 0;
}
