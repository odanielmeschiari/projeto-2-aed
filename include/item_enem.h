#ifndef ITEM_ENEM_H
#define ITEM_ENEM_H

typedef struct item_enem ITEM_ENEM;

ITEM_ENEM *item_enem_criar(const char *linha);
void item_enem_apagar(ITEM_ENEM **item);
size_t item_enem_numero_campos(const ITEM_ENEM *item);
const char *item_enem_get_campo(const ITEM_ENEM *item, size_t indice);
const char *item_enem_get_linha(const ITEM_ENEM *item);

#endif
