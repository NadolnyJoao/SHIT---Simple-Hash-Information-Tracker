#ifndef STATUS_H
#define STATUS_H

typedef enum {
  MUDANCA_NOVO,
  MUDANCA_MODIFICADO,
  MUDANCA_REMOVIDO
} TipoMudanca;

typedef struct {
  char caminho[256];
  TipoMudanca tipo;
} Mudanca;

void Status();
int Encontra_Mudancas(Mudanca **lista, int *qtd);

#endif