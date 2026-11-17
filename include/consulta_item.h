#ifndef CONSULTA_ITEM_H
#define CONSULTA_ITEM_H

typedef struct consulta_item CONSULTA_ITEM;

CONSULTA_ITEM *consulta_item_criar(const char *linha);
void consulta_item_apagar(CONSULTA_ITEM **consulta);
const char *consulta_item_get_campo(const CONSULTA_ITEM *consulta);
const char *consulta_item_get_valor(const CONSULTA_ITEM *consulta);

#endif
