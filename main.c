#include "sha1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "commit.h"
#include "status.h"
#include "file_utils.h"

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
  }else if (strcmp(argv[1], "commit") == 0)
  {
    CreateCommit(argv,argc);
  }else if(strcmp(argv[1],"status") == 0){
    Status();
  }
  // Comando desconhecido nao e tratado: o programa apenas encerra sem aviso.
  return 0;
}

void cmd_init() {
  // stat() verifica se ".shit" ja existe, evitando reinicializar o repositorio.
  struct stat st;
  if (stat(".shit", &st) == 0) {
    printf("Repositorio SHIT ja existente.\n");
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
    fclose(head);
  }

  printf("Repositorio SHIT inicializado com sucesso.\n");
}

// Placeholder: ainda imprime dados fixos, sem ler os commits reais.
void RenderLogs() {
  printf("mensagem hash \nAuthor: me\nData: 16-10-2026\n\n   Name mensagem ");
}


// Implementa "add": calcula o hash do arquivo, salva uma copia em
// .shit/objects/<hash> e registra "caminho hash" no staging (.shit/index).
void cmd_add(const char *path) {
  char *hash = calcula_sha1(path);
  if (hash == NULL) {
    printf("Erro ao ler o arquivo '%s'. \n", path);
    return;
  }
  long tamanho;
  char object_path[128];
  // O nome do objeto e o proprio hash: conteudos iguais geram o mesmo arquivo
  // (deduplicacao natural, como no Git).
  snprintf(object_path, sizeof(object_path), ".shit/objects/%s", hash);
  char *buffer = read_file(path, &tamanho);
  if (buffer == NULL) {
    printf("Erro: adicione um arquivo valido");
    free(hash);
    return;
  }
  // Grava o conteudo do arquivo dentro do objeto.
  FILE *dest_file = fopen(object_path, "wb");
  if (dest_file == NULL) {
    printf("Erro");
    free(hash);
    free(buffer);
    return;
  }
  fwrite(buffer, 1, tamanho, dest_file);
  fclose(dest_file);
  // Modo "a" (append): adiciona a linha ao index sem apagar o que ja existe.
  FILE *staging = fopen(".shit/index", "a");
  if (staging == NULL) {
    free(hash);
    free(buffer);
    printf("ERRO!");
    return;
  }
  // Separador '\t' (tab) para permitir nomes de arquivo com espacos.
  fprintf(staging, "%s\t%s\n", path, hash);
  fclose(staging);
  free(hash);
  free(buffer);
}
