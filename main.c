#include "sha1.h"
#include <stdio.h>
#include <stdlib.h>
#include "commit.h"
#include <string.h>
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

void cmd_init();
void RenderLogs();
char *read_file(const char *path, long *out_size);
char *calcula_sha1(const char *path);
void cmd_add(const char *filename);

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("SHIT - Simple Hash Information Tracker\n\n");
    printf("Uso:\n");
    printf("  shit init             Inicializa um repositorio\n");
    printf("  shit add <arquivo>    Adiciona um arquivo a area de preparacao\n");
    printf("  shit log              Exibe o historico de commits\n");
    return 0;
  }
  if (strcmp(argv[1], "init") == 0) {
    cmd_init();
  } else if (strcmp(argv[1], "log") == 0) {
    RenderLogs();
  } else if (strcmp(argv[1], "add") == 0) {
    if (argc < 3) {
      fprintf(stderr, "shit: erro: nenhum arquivo informado.\n");
      fprintf(stderr, "uso: shit add <arquivo>\n");
      return 1;
    }
    cmd_add(argv[2]);
  }else if (strcmp(argv[1], "commit") == 0)
  {
    CreateCommit(argv,argc);
  } else {
    fprintf(stderr, "shit: erro: comando desconhecido '%s'.\n", argv[1]);
    fprintf(stderr, "uso: shit init | shit add <arquivo> | shit log\n");
    return 1;
  }
  return 0;
}

void cmd_init() {
  // stat() verifica se ".shit" ja existe, evitando reinicializar o repositorio.
  struct stat st;
  if (stat(".shit", &st) == 0) {
    printf("Repositorio SHIT ja inicializado em '.shit/'.\n");
    return;
  }
  MAKE_DIR(".shit");
  MAKE_DIR(".shit/objects");
  // MAKE_DIR(".shit/index");
  MAKE_DIR(".shit/commits");

  // HEAD e criado vazio; sera preenchido a medida que os commits forem feitos.
  FILE *head = fopen(FILE_HEAD, "w");
  if (head != NULL) {
    fprintf(head,"break");// coloquei esse dado porq precisa de um previus quando inicia commit 
  if (head == NULL) {
    fprintf(stderr, "shit: aviso: nao foi possivel criar '%s'.\n", FILE_HEAD);
  } else {
    fclose(head);
  }

  printf("Repositorio SHIT inicializado em '.shit/'.\n");
}

// Placeholder: ainda imprime dados fixos, sem ler os commits reais.
void RenderLogs() {
  printf("mensagem hash \nAuthor: me\nData: 16-10-2026\n\n   Name mensagem ");
}

// Le o arquivo inteiro para a memoria e devolve um buffer alocado pelo chamador
// (que deve dar free) e, em *out_size, o numero de bytes lidos.
// Retorna NULL em caso de erro. O buffer NAO termina com '\0', por isso sempre
// use o tamanho retornado junto com os dados.
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
// Calcula o SHA-1 do conteudo do arquivo e devolve a representacao em
// hexadecimal (40 caracteres + '\0'). O chamador deve liberar com free.
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

// Implementa "add": calcula o hash do arquivo, salva uma copia em
// .shit/objects/<hash> e registra "caminho hash" no staging (.shit/index).
void cmd_add(const char *path) {
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
    fprintf(stderr, "shit: erro: nao foi possivel gravar o objeto '%s'.\n", object_path);
    free(hash);
    free(buffer);
    return;
  }
  fwrite(buffer, 1, tamanho, dest_file);
  fclose(dest_file);
  // Modo "a" (append): adiciona a linha ao index sem apagar o que ja existe.
  FILE *staging = fopen(".shit/index", "a");
  if (staging == NULL) {
    fprintf(stderr, "shit: erro: nao foi possivel abrir o indice '.shit/index'.\n");
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
