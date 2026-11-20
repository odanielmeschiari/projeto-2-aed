#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>

typedef struct lista LISTA;

LISTA *lista_criar(void);
void lista_apagar(LISTA **x);
bool lista_inserir(LISTA *x, long long offset);
long long lista_get_offset(LISTA *x, long long indice);
long long lista_tamanho(LISTA *x);

#endif