#include <stdlib.h>
#include <stdio.h>

#include "file_utils.h"
#include "sha1.h"


char *read_file(const char *path, long *out_size) {
  FILE *file = fopen(path, "rb"); // "rb" le em modo binario, sem traduzir quebras de linha
  if (file == NULL)
    return NULL;
  fseek(file, 0, SEEK_END); // vai para o fim para medir o tamanho
  long size = ftell(file);
  rewind(file); // volta ao inicio para ler
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
  unsigned char digest[20]; // SHA-1 gera 160 bits = 20 bytes
  SHA1Init(&contexto);
  SHA1Update(&contexto, (unsigned char *)buffer, size);
  SHA1Final(digest, &contexto);
  free(buffer);
  // 20 bytes em hex = 40 caracteres; +1 para o terminador.
  char *result_hash = malloc(41);
  if (result_hash == NULL)
    return NULL;
  int i;
  for (i = 0; i < 20; i++) {
    sprintf(&result_hash[i * 2], "%02x", digest[i]); // cada byte vira 2 digitos
  }
  result_hash[40] = '\0';
  return result_hash;
}