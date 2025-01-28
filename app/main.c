#include <dirent.h>
#include <stdbool.h>
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
      return envPaths[i];
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

  printf("%s is %s/%s\n", str, fullPath, str);
}

void systemOtherFunction(char *operation, char *params, char **envPaths) {

  char *fullPath = getFunctionPath(operation, envPaths);
  if (!fullPath) {
    fprintf(stderr, "%s: command not found\n", operation);
    return;
  }

  FILE *fp;
  char data[PATH_MAX];
  char command[PATH_MAX];
  snprintf(command, sizeof(command), "PATH=%s %s %s", fullPath, operation,
           params ? params : "");

  /* printf("%s", command); */
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
  if (path[0] == '~') {
    path++;
    char *home = getenv("HOME");
    strcat(home, path);
    path = home;
  }

  if (access(path, F_OK) == 0) {
    chdir(path);
    // setenv("PWD", path, 0);
  } else
    printf("cd: %s: No such file or directory\n", path);
}

char **getEnvPaths() {
  char *envString = getenv("PATH");
  /* printf("%s ", envString); */

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

void shiftLeftFromPosition(char **input, int position) {
  for (int i = position; strlen(*input); i++) {
    if (strlen(*input) <= i + 1) {
      (*input)[i] = '\0';
      break;
    }
    (*input)[i] = (*input)[i + 1];
  }
}

void shiftRightFromPosition(char **input, int position) {
  char *output = malloc(sizeof(char) * (strlen(*input) + 1));
  strcpy(output, *input);
  output[position] = ' ';
  for (int i = position; i < strlen(*input); i++) {
    output[i + 1] = (*input)[i];
  }
  *input = output;
}

void removeSingleQuotes(char **input, bool escChar) {
  int size = strlen(*input);
  bool singelQoute = false;
  bool doubleQuote = false;
  bool trailingSpace = false;
  for (int i = 0; i < size; i++) {
    if ((*input)[i] == '\'' && !doubleQuote) {
      singelQoute = !singelQoute;
      shiftLeftFromPosition(input, i);
      size--;
      i--;
      continue;
    }
    if ((*input)[i] == '\"' && !singelQoute) {
      doubleQuote = !doubleQuote;
      shiftLeftFromPosition(input, i);
      size--;
      i--;
      continue;
    }
    if ((*input)[i] == '\\' && (!singelQoute && !doubleQuote)) {
      shiftLeftFromPosition(input, i);
      size--;
      continue;
    }
    if ((*input)[i] == ' ' && singelQoute && escChar) {
      shiftRightFromPosition(input, i);
      (*input)[i] = '\\';
      size++;
      i++;
      continue;
    }
    if ((*input)[i] == ' ' && (!singelQoute && !doubleQuote)) {
      if (trailingSpace) {
        shiftLeftFromPosition(input, i);
        size--;
        i--;
      }
      trailingSpace = true;
      continue;
    }
    trailingSpace = false;
  }
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
      removeSingleQuotes(&reminder, false);
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
      /* removeSingleQuotes(&reminder, true); */
      systemOtherFunction(operation, reminder, envPaths);
    }
    fflush(stdout);
  } while (1);
  cleanup(envPaths);
  return 0;
}
