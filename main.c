#include "sha1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MAKE_DIR(p) _mkdir(p)
#else
#define MAKE_DIR(p) mkdir((p), 0777)
#endif

#include <time.h>
#define FILE_HEAD ".shit/HEAD"

void cmd_init();
void RenderLogs();
char *read_file(const char *path, long *out_size);
char *calcula_sha1(const char *path);
void cmd_add(const char *filename);

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("SHIT - A good version of Git");
    return 0;
  }
  if (strcmp(argv[1], "init") == 0) {
    cmd_init();
  } else if (strcmp(argv[1], "log") == 0) {
    RenderLogs();
  } else if (strcmp(argv[1], "add") == 0) {
    if (argc < 3) {
      printf("ERRO: informe o arquivo\n");
      return 0;
    }
    // char *hash = calcula_sha1(argv[2]);
    cmd_add(argv[2]);
  }
  return 0;
}

void cmd_init() {
  struct stat st;
  if (stat(".shit", &st) == 0) {
    printf("Repositorio SHIT ja existente.\n");
    return;
  }
  MAKE_DIR(".shit");
  MAKE_DIR(".shit/objects");
  MAKE_DIR(".shit/commits"); // Pasta para guardar os objetos de commit

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

void cmd_add(const char *path) {
  char *hash = calcula_sha1(path);
  if (hash == NULL) {
    printf("Erro ao ler o arquivo '%s'. \n", path);
    return;
  }
  long tamanho;
  char object_path[128];
  snprintf(object_path, sizeof(object_path), ".shit/objects/%s", hash);
  char *buffer = read_file(path, &tamanho);
  if (buffer == NULL) {
    printf("Erro: adicione um arquivo valido");
    free(hash);
    return;
  }
  FILE *dest_file = fopen(object_path, "wb");
  if (dest_file == NULL) {
    printf("Erro");
    free(hash);
    free(buffer);
    return;
  }
  fwrite(buffer, 1, tamanho, dest_file);
  fclose(dest_file);
  FILE *staging = fopen(".shit/index", "a");
  if (staging == NULL) {
    free(hash);
    free(buffer);
    printf("ERRO!");
    return;
  }
  fprintf(staging, "%s %s\n", path, hash);
  fclose(staging);
  free(hash);
  free(buffer);
}
