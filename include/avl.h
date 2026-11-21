#ifndef AVL_H
#define AVL_H

#include <stdbool.h>
#include "lista.h"

typedef struct avl AVL;

AVL *avl_criar(const char *criterio);
void avl_apagar(AVL **x);
bool avl_inserir(AVL *x, const char *chave, long long offset);
LISTA *avl_busca(AVL *x, const char *chave);
int avl_altura(AVL *x);
int avl_num_chaves(AVL *x);
bool avl_salvar_indice(AVL *x, FILE *f);
AVL *avl_recuperar_indice(FILE *f);

#endif