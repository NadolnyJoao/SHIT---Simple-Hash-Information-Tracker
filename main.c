#include "add.h"
#include "commit.h"
#include "init.h"
#include "status.h"
#include <stdio.h>
#include <stdlib.h>
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
// Caminho do arquivo HEAD, que aponta para o commit atual (ainda nao usado).
#define FILE_HEAD ".shit/HEAD"

void RenderLogs();

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("SHIT - Simple Hash Information Tracker\n\n");
    printf("Uso:\n");
    printf("  shit init             Inicializa um repositorio\n");
    printf(
        "  shit add <arquivo>    Adiciona um arquivo a area de preparacao\n");
    printf("  shit log              Exibe o historico de commits\n");
    return 0;
  }
  if (strcmp(argv[1], "init") == 0) {
    shit_init();
  } else if (strcmp(argv[1], "log") == 0) {
    RenderLogs();
  } else if (strcmp(argv[1], "add") == 0) {
    if (argc < 3) {
      fprintf(stderr, "shit: erro: nenhum arquivo informado.\n");
      fprintf(stderr, "uso: shit add <arquivo>\n");
      return 1;
    }
    if (strcmp(argv[2], ".") == 0) {
      shit_add_all();
    } else {
      shit_add(argv[2]);
    }
  } else if (strcmp(argv[1], "register") == 0) {
    CreateCommit(argv, argc);
  } else if (strcmp(argv[1], "revert") == 0) {
    if (argc < 3) {
      fprintf(stderr, "shit: erro: nenhum commit informado.\n");
      fprintf(stderr, "uso: shit revert <commit-id>\n");
      return 1;
    }
    RevertCommit(argv[2]);
  } else if (strcmp(argv[1], "status") == 0) {
    Status();
  } else {
    fprintf(stderr, "shit: erro: comando desconhecido '%s'.\n", argv[1]);
    fprintf(stderr, "uso: shit init | shit add <arquivo> | shit log\n");
    return 1;
  }
  return 0;
}

void RenderLogs() {
  char *first = GetLastCommit();
  if (first == NULL) {
    printf("Nenhum commit encontrado no historico.\n");
    return;
  }
  first[strcspn(first, "\r\n")] = '\0';
  if (strcmp(first, "break") == 0 || strlen(first) == 0) {
    printf("Nenhum commit encontrado no historico.\n");
    free(first);
    return;
  }

  char cur_hash[255];
  strncpy(cur_hash, first, 254);
  cur_hash[254] = '\0';
  free(first);

  while (strcmp(cur_hash, "break") != 0 && strlen(cur_hash) > 0) {
    Commit *c = ReadCommit(cur_hash);
    if (c == NULL) {
      break;
    }
    printf("commit %s\n", c->id_hash);
    printf("Date:   %s\n", c->date);
    printf("        %s\n\n", c->menssage);

    strncpy(cur_hash, c->previous, 254);
    cur_hash[254] = '\0';
    cur_hash[strcspn(cur_hash, "\r\n")] = '\0';

    free(c->files);
    free(c);

    if (strcmp(cur_hash, "break") == 0 || strlen(cur_hash) == 0) {
      break;
    }
  }
}
