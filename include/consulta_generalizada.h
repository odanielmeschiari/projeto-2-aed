#ifndef CONSULTA_GENERALIZADA_H
#define CONSULTA_GENERALIZADA_H

typedef struct consulta_generalizada CONSULTA_GENERALIZADA;

CONSULTA_GENERALIZADA *consulta_generalizada_criar(const char *linha, const char *cabecalho);
void consulta_generalizada_apagar(CONSULTA_GENERALIZADA **consulta);
const char *consulta_generalizada_get_campo(const CONSULTA_GENERALIZADA *consulta);
const char *consulta_generalizada_get_valor(const CONSULTA_GENERALIZADA *consulta);
size_t consulta_generalizada_get_indice(const CONSULTA_GENERALIZADA *consulta);

#endif
