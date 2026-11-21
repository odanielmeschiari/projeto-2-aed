#ifndef ARVORE_H
#define ARVORE_H

#include "abb.h"
#include "arvore.h"
#include "rn.h"
#include "lista.h"

#include <stdbool.h>

void *arvore_criar(const char *tipo, const char *criterio);
void arvore_apagar(const char *tipo, void **x);
bool arvore_inserir(const char *tipo, void *x, const char *chave, long long offset);
LISTA *arvore_busca(const char *tipo, void *x, const char *chave);
int arvore_altura(const char *tipo, void *x);
int arvore_num_chaves(const char *tipo, void *x);

#endif