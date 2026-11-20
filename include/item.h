#ifndef ITEM_H
#define ITEM_H

#include <stddef.h>

typedef struct item ITEM;

ITEM *item_criar(const char *linha, const char *cabecalho, const char *campo, long long offset);
void item_apagar(ITEM **item);
const char *item_get_linha(const ITEM *item);
const char *item_get_campo(const ITEM *item);
const char *item_get_valor(const ITEM *item);
size_t item_get_indice(const ITEM *item);
long long item_get_offset(const ITEM *item);
int item_cmp(const ITEM *a, const ITEM *b, const char *criterio);

#endif