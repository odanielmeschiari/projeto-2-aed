#ifndef ITEM_GENERALIZADO_H
#define ITEM_GENERALIZADO_H

#include <stddef.h>

typedef struct item_generalizado ITEM_GENERALIZADO;

ITEM_GENERALIZADO *item_generalizado_criar(const char *linha, const char *cabecalho, const char *campo, long long offset);
void item_generalizado_apagar(ITEM_GENERALIZADO **item);
const char *item_generalizado_get_linha(const ITEM_GENERALIZADO *item);
const char *item_generalizado_get_campo(const ITEM_GENERALIZADO *item);
const char *item_generalizado_get_valor(const ITEM_GENERALIZADO *item);
size_t item_generalizado_get_indice(const ITEM_GENERALIZADO *item);
long long item_generalizado_get_offset(const ITEM_GENERALIZADO *item);

#endif
