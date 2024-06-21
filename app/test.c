

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[]) {
  FILE *fp;
  char *envPaths = getenv("PATH");

  int status;

  char data[1024];

  fp = popen("ls ~", "r");
  if (fp == NULL) {
    printf("Error");
  }

  while (fgets(data, 1024, fp) != NULL)
    printf("console :%s \n", data);

  status = pclose(fp);

  return EXIT_SUCCESS;
}
