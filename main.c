#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#define FILE_HEAD ".shit/HEAD"

void cmd_init();
void RenderLogs();

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("SHIT - A good version of Git");
    return 0;
  }
  if (strcmp(argv[1], "log") == 0) {
    RenderLogs();
  }
  return 0;
}

void cmd_init() {

  mkdir(".shit", 0777);
  mkdir(".shit/objects", 0777);
  FILE *head = fopen(FILE_HEAD, "w");
  fclose(head);
}

void RenderLogs() {
  printf("mensagem hash \nAuthor: me\nData: 16-10-2026\n\n   Name mensagem ");
}
