#ifndef LISTA_H
#define LISTA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct lista LISTA;

LISTA *lista_criar(void);
void lista_apagar(LISTA **x);
bool lista_inserir(LISTA *x, long long offset);
long long lista_get_offset(LISTA *x, long long indice);
long long lista_tamanho(LISTA *x);
void lista_salvar_indice(LISTA *x, FILE *f);
LISTA *lista_carregar_indice(FILE *f);

#endif