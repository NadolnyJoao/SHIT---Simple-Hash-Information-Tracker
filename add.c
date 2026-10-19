#include "file_utils.h"
#include "init.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

// Abstrai a criacao de diretorios: no Windows e _mkdir, no Linux/Mac usa mkdir.
// Ambos recebem o modo 0777 (permissoes rwx para todos os usuarios).
#ifdef _WIN32
#include <direct.h>
#define MAKE_DIR(p) _mkdir(p)
#else
#define MAKE_DIR(p) mkdir((p), 0777)
#endif

#include <time.h>
// Caminho do arquivo HEAD, que aponta para o commit atual (ainda nao usado).
#define FILE_HEAD ".shit/HEAD"

// Implementa "add": calcula o hash do arquivo, salva uma copia em
// .shit/objects/<hash> e registra "caminho hash" no staging (.shit/index).
void shit_add(const char *path) {
  char *hash = calcula_sha1(path);
  if (hash == NULL) {
    fprintf(stderr, "shit: erro: nao foi possivel abrir ou ler '%s'.\n", path);
    return;
  }
  long tamanho;
  char object_path[128];
  // O nome do objeto e o proprio hash: conteudos iguais geram o mesmo arquivo
  // (deduplicacao natural, como no Git).
  snprintf(object_path, sizeof(object_path), ".shit/objects/%s", hash);
  char *buffer = read_file(path, &tamanho);
  if (buffer == NULL) {
    fprintf(stderr, "shit: erro: falha ao ler o conteudo de '%s'.\n", path);
    free(hash);
    return;
  }
  // Grava o conteudo do arquivo dentro do objeto.
  FILE *dest_file = fopen(object_path, "wb");
  if (dest_file == NULL) {
    fprintf(stderr, "shit: erro: nao foi possivel gravar o objeto '%s'.\n",
            object_path);
    free(hash);
    free(buffer);
    return;
  }
  fwrite(buffer, 1, tamanho, dest_file);
  fclose(dest_file);
  // Modo "a" (append): adiciona a linha ao index sem apagar o que ja existe.
  FILE *staging = fopen(".shit/index", "a");
  if (staging == NULL) {
    fprintf(stderr,
            "shit: erro: nao foi possivel abrir o indice '.shit/index'.\n");
    free(hash);
    free(buffer);
    return;
  }
  // Separador '\t' (tab) para permitir nomes de arquivo com espacos.
  fprintf(staging, "%s\t%s\n", path, hash);
  fclose(staging);
  printf("adicionado: %s (%s)\n", path, hash);
  free(hash);
  free(buffer);
}
