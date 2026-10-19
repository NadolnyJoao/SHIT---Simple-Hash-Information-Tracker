#include <stdio.h>
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

void shit_init() {
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
    fprintf(head, "break"); // coloquei esse dado porq precisa de um previus
                            // quando inicia commit
    if (head == NULL) {
      fprintf(stderr, "shit: aviso: nao foi possivel criar '%s'.\n", FILE_HEAD);
    } else {
      fclose(head);
    }

    printf("Repositorio SHIT inicializado em '.shit/'.\n");
  }
}
