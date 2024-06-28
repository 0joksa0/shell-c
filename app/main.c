#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define EXIT 96082464191
#define ECHO 96082375396
#define TYPE 96086588231
#define PWD 1478238160
#define CD 22741260

#define PATH_MAX 4096

int size = 0;
char cwd[1024];

unsigned long hash(const char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash * 33) + c;
  return hash;
}

char *getFunctionPath(char *function, char **envPaths) {
  if (!function || !envPaths) {
    return NULL;
  }

  static char fullPath[PATH_MAX];

  for (int i = 0; i < size; i++) {
    snprintf(fullPath, PATH_MAX, "%s/%s", envPaths[i], function);
    if (access(fullPath, X_OK) == 0) {
      return fullPath;
    }
  }
  return NULL;
}

void typeFunction(char *str, char **envPaths) {
  unsigned long hashCode = hash(str);
  if (hashCode == TYPE || hashCode == EXIT || hashCode == ECHO ||
      hashCode == PWD) {
    printf("%s is a shell builtin\n", str);
    return;
  }

  char *fullPath = getFunctionPath(str, envPaths);

  if (!fullPath) {
    printf("%s: not found\n", str);
    return;
  }

  printf("%s is %s\n", str, fullPath);
}

void systemOtherFunction(char *operation, char *params, char **envPaths) {

  char *fullPath = getFunctionPath(operation, envPaths);

  if (!fullPath) {
    printf("%s: not found\n", operation);
    return;
  }

  FILE *fp;
  char data[PATH_MAX];
  char command[PATH_MAX];
  snprintf(command, sizeof(command), "%s %s", fullPath, params ? params : "");

  // printf("%s", command);
  fp = popen(command, "r");
  if (fp == NULL) {
    printf("Error running a command");
  }

  while (fgets(data, 1024, fp) != NULL)
    printf("%s", data);

  pclose(fp);

  return;
}

void cdFunction(char *path) {
  if (!path) {
    printf("Error");
  }

  if (access(path, F_OK) == 0) {
    chdir(path);
    // setenv("PWD", path, 0);
  } else
    printf("cd: %s: No such file or directory\n", path);
}

char **getEnvPaths() {
  char *envString = getenv("PATH");

  if (!envString || strlen(envString) == 0) {
    return NULL;
  }

  size = 1;
  for (int i = 0; i < strlen(envString); i++) {
    size += envString[i] == ':' ? 1 : 0;
  }

  char **list = (char **)malloc(size * sizeof(char *));
  if (!list) {
    perror("malloc");
    return NULL;
  }

  char *p = strtok(envString, ":");
  for (int i = 0; i < size; i++) {
    list[i] = p;

    if (!list[i]) {
      perror("Strdup error encountered");
      for (int j = 0; j < i; j++)
        free(list[j]);
      free(list);
      return NULL;
    }

    p = strtok(NULL, ":");
  }

  return list;
}

void cleanup(char **envPaths) { free(envPaths); }

int main(int argc, char *argv[]) {

  // Wait for user input
  char input[100];
  char **envPaths = getEnvPaths();
  if (!envPaths) {
    fprintf(stderr, "Failed to get EnvPaths! \n");
    return EXIT_FAILURE;
  }

  do {
    printf("$ ");
    fflush(stdout);

    fgets(input, 100, stdin);
    input[strcspn(input, "\n")] = 0;
    char *reminder;
    char *operation = strtok_r(input, " ", &reminder);

    // printf("%lu", hash(operation));
    switch (hash(operation)) {
    case EXIT:
      cleanup(envPaths);
      exit(0);
      break;
    case ECHO:
      printf("%s\n", reminder);

      break;
    case TYPE:
      typeFunction(reminder, envPaths);
      break;
    case PWD:
      getcwd(cwd, sizeof(cwd));
      printf("%s\n", cwd);
      break;
    case CD:
      cdFunction(reminder);
      break;
    default:
      systemOtherFunction(operation, reminder, envPaths);
    }

  } while (1);
  cleanup(envPaths);
  return 0;
}
