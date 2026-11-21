#ifndef RN_H
#define RN_H

#include <stdbool.h>
#include "lista.h"

typedef struct rn RN;

RN *rn_criar(const char *criterio);
void rn_apagar(RN **x);
bool rn_inserir(RN *x, const char *chave, long long offset);
LISTA *rn_busca(RN *x, const char *chave);
int rn_altura(RN *x);
int rn_num_chaves(RN *x);
bool rn_salvar_indice(RN *x, FILE *f);
RN *rn_recuperar_indice(FILE *f);

#endif