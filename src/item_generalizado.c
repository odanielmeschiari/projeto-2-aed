#include "item_generalizado.h"

#include <stdlib.h>
#include <string.h>

struct item_generalizado {
	char *linha;
	char *campo;
	char *valor;
	size_t indice;
	long long offset;
};

static char *duplicar(const char *texto, size_t tamanho){
	char *copia = (char *)malloc(tamanho + 1);
	if (copia == NULL) return NULL;
	memcpy(copia, texto, tamanho);
	copia[tamanho] = '\0';
	return copia;
}

static char delimitar(const char *cabecalho){
	if (cabecalho == NULL) return ',';
	return strchr(cabecalho, ';') != NULL ? ';' : ',';
}

static char *obter_campo(const char *linha, const char *cabecalho, const char *campo, size_t *indice){
	if (linha == NULL || cabecalho == NULL || campo == NULL){
		if (indice != NULL) *indice = (size_t)-1;
		return NULL;
	}

	char delimitador = delimitar(cabecalho);
	size_t quantidade = 1;
	for (const char *p = cabecalho; *p != '\0'; p++){
		if (*p == delimitador) quantidade++;
	}

	char *cabecalho_copia = duplicar(cabecalho, strlen(cabecalho));
	char *linha_copia = duplicar(linha, strcspn(linha, "\r\n"));
	if (cabecalho_copia == NULL || linha_copia == NULL){
		free(cabecalho_copia);
		free(linha_copia);
		if (indice != NULL) *indice = (size_t)-1;
		return NULL;
	}

	size_t alvo = quantidade;
	char *inicio_coluna = cabecalho_copia;
	for (size_t i = 0; i < quantidade; i++){
		char *fim_coluna = strchr(inicio_coluna, delimitador);
		if (fim_coluna != NULL) *fim_coluna = '\0';
		if (strcmp(inicio_coluna, campo) == 0){
			alvo = i;
			break;
		}
		if (fim_coluna == NULL) break;
		inicio_coluna = fim_coluna + 1;
	}
	if (alvo == quantidade){
		free(cabecalho_copia);
		free(linha_copia);
		if (indice != NULL) *indice = (size_t)-1;
		return NULL;
	}

	char *inicio_valor = linha_copia;
	char *valor = NULL;
	for (size_t i = 0; i <= alvo; i++){
		char *fim_valor = strchr(inicio_valor, delimitador);
		if (i == alvo){
			if (fim_valor != NULL) *fim_valor = '\0';
			if (*inicio_valor == '"' && inicio_valor[strlen(inicio_valor) - 1] == '"'){
				inicio_valor++;
				inicio_valor[strlen(inicio_valor) - 1] = '\0';
			}
			valor = duplicar(inicio_valor, strlen(inicio_valor));
			break;
		}
		if (fim_valor == NULL){
			valor = NULL;
			break;
		}
		inicio_valor = fim_valor + 1;
	}

	free(cabecalho_copia);
	free(linha_copia);
	if (indice != NULL) *indice = alvo;
	return valor;
}

ITEM_GENERALIZADO *item_generalizado_criar(const char *linha, const char *cabecalho, const char *campo, long long offset){
	if (linha == NULL || campo == NULL) return NULL;

	ITEM_GENERALIZADO *item = (ITEM_GENERALIZADO *)malloc(sizeof(ITEM_GENERALIZADO));
	if (item == NULL) return NULL;
	item->linha = NULL;
	item->campo = NULL;
	item->valor = NULL;
	item->indice = (size_t)-1;
	item->offset = offset;

	size_t tamanho_linha = strcspn(linha, "\r\n");
	item->linha = duplicar(linha, tamanho_linha);
	item->campo = duplicar(campo, strlen(campo));
	item->valor = obter_campo(linha, cabecalho, campo, &item->indice);
	if (item->linha == NULL || item->campo == NULL || item->valor == NULL){
		item_generalizado_apagar(&item);
		return NULL;
	}
	return item;
}

void item_generalizado_apagar(ITEM_GENERALIZADO **item){
	if (item == NULL || *item == NULL) return;
	free((*item)->linha);
	free((*item)->campo);
	free((*item)->valor);
	free(*item);
	*item = NULL;
}

const char *item_generalizado_get_linha(const ITEM_GENERALIZADO *item){
	if (item == NULL) return NULL;
	return item->linha;
}

const char *item_generalizado_get_campo(const ITEM_GENERALIZADO *item){
	if (item == NULL) return NULL;
	return item->campo;
}

const char *item_generalizado_get_valor(const ITEM_GENERALIZADO *item){
	if (item == NULL) return NULL;
	return item->valor;
}

size_t item_generalizado_get_indice(const ITEM_GENERALIZADO *item){
	if (item == NULL) return (size_t)-1;
	return item->indice;
}

long long item_generalizado_get_offset(const ITEM_GENERALIZADO *item){
	if (item == NULL) return -1;
	return item->offset;
}
