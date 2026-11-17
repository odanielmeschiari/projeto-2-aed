#ifndef CONSULTA_ENEM_H
#define CONSULTA_ENEM_H

typedef struct consulta_enem CONSULTA_ENEM;

CONSULTA_ENEM *consulta_enem_criar(const char *linha, const char *cabecalho);
void consulta_enem_apagar(CONSULTA_ENEM **consulta);
const char *consulta_enem_get_campo(const CONSULTA_ENEM *consulta);
const char *consulta_enem_get_valor(const CONSULTA_ENEM *consulta);
size_t consulta_enem_get_indice(const CONSULTA_ENEM *consulta);

#endif
