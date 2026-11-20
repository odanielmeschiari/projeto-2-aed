#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include "lista.h"

typedef struct abb ABB;

ABB *abb_criar(const char *criterio);
void abb_apagar(ABB **x);
bool abb_inserir(ABB *x, const char *chave, long long offset);
LISTA *abb_busca(ABB *x, const char *chave);
int abb_altura(ABB *x);
int abb_num_chaves(ABB *x);

#endif