#include "consulta_enem.h"

#include "consulta_item.h"
#include "item_enem.h"

#include <stdlib.h>
#include <string.h>

struct consulta_enem {
	CONSULTA_ITEM *item;
	size_t indice;
};

CONSULTA_ENEM *consulta_enem_criar(const char *linha, const char *cabecalho){
	if (linha == NULL || cabecalho == NULL) return NULL;

	CONSULTA_ITEM *item = consulta_item_criar(linha);
	ITEM_ENEM *colunas = item_enem_criar(cabecalho);
	if (item == NULL || colunas == NULL){
		consulta_item_apagar(&item);
		item_enem_apagar(&colunas);
		return NULL;
	}

	size_t quantidade = item_enem_numero_campos(colunas);
	size_t indice = quantidade;
	for (size_t i = 0; i < quantidade; i++){
		const char *coluna = item_enem_get_campo(colunas, i);
		const char *campo = consulta_item_get_campo(item);
		if (strcmp(coluna, campo) == 0){
			indice = i;
			break;
		}
	}
	item_enem_apagar(&colunas);
	if (indice == quantidade){
		consulta_item_apagar(&item);
		return NULL;
	}

	CONSULTA_ENEM *consulta = (CONSULTA_ENEM *)malloc(sizeof(CONSULTA_ENEM));
	if (consulta == NULL){
		consulta_item_apagar(&item);
		return NULL;
	}
	consulta->item = item;
	consulta->indice = indice;
	return consulta;
}

void consulta_enem_apagar(CONSULTA_ENEM **consulta){
	if (consulta == NULL || *consulta == NULL) return;
	consulta_item_apagar(&(*consulta)->item);
	free(*consulta);
	*consulta = NULL;
}

const char *consulta_enem_get_campo(const CONSULTA_ENEM *consulta){
	if (consulta == NULL) return NULL;
	return consulta_item_get_campo(consulta->item);
}

const char *consulta_enem_get_valor(const CONSULTA_ENEM *consulta){
	if (consulta == NULL) return NULL;
	return consulta_item_get_valor(consulta->item);
}

size_t consulta_enem_get_indice(const CONSULTA_ENEM *consulta){
	if (consulta == NULL) return (size_t)-1;
	return consulta->indice;
}
