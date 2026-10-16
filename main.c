#include "sha1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#define FILE_HEAD ".shit/HEAD"

void cmd_init();
void RenderLogs();
char *read_file(const char *path, long *out_size);
char *calcula_sha1(const char *path);

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("SHIT - A good version of Git");
    return 0;
  }
  if (strcmp(argv[1], "init") == 0) {
    cmd_init();
  } else if (strcmp(argv[1], "log") == 0) {
    RenderLogs();
  }
  return 0;
}

void cmd_init() {
  struct stat st;
  if (stat(".shit", &st) == 0) {
    printf("Repositorio SHIT ja existente.\n");
    return;
  }
  mkdir(".shit", 0777);
  mkdir(".shit/objects", 0777);

  FILE *head = fopen(FILE_HEAD, "w");
  if (head != NULL) {
    fclose(head);
  }

  printf("Repositorio SHIT inicializado com sucesso.\n");
}

void RenderLogs() {
  printf("mensagem hash \nAuthor: me\nData: 16-10-2026\n\n   Name mensagem ");
}

char *read_file(const char *path, long *out_size) {
  FILE *file = fopen(path, "rb");
  if (file == NULL)
    return NULL;
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);
  char *buffer = (char *)malloc(size);
  if (buffer == NULL) {
    fclose(file);
    return NULL;
  }
  fread(buffer, 1, size, file);
  fclose(file);
  if (out_size != NULL) {
    *out_size = size;
  }
  return buffer;
}
char *calcula_sha1(const char *path) {
  long size;
  char *buffer = read_file(path, &size);
  if (buffer == NULL) {
    return NULL;
  }
  SHA1_CTX contexto;
  unsigned char digest[20];
  SHA1Init(&contexto);
  SHA1Update(&contexto, (unsigned char *)buffer, size);
  SHA1Final(digest, &contexto);
  free(buffer);
  char *result_hash = malloc(41);
  if (result_hash == NULL)
    return NULL;
  int i;
  for (i = 0; i < 20; i++) {
    sprintf(&result_hash[i * 2], "%02x", digest[i]);
  }
  result_hash[40] = '\0';
  return result_hash;
}
