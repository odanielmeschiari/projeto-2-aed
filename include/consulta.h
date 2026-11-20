#ifndef CONSULTA_H
#define CONSULTA_H

typedef struct consulta CONSULTA;

CONSULTA *consulta_criar(const char *linha, const char *cabecalho);
void consulta_apagar(CONSULTA **consulta);
const char *consulta_get_campo(const CONSULTA *consulta);
const char *consulta_get_valor(const CONSULTA *consulta);
size_t consulta_get_indice(const CONSULTA *consulta);

#endif
