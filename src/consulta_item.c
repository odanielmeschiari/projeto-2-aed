#include "consulta_item.h"

#include <stdlib.h>
#include <string.h>

struct consulta_item {
	char *campo;
	char *valor;
};

static char *duplicar_texto(const char *texto, size_t tamanho){
	char *copia = (char *)malloc(tamanho + 1);
	if (copia == NULL) return NULL;
	memcpy(copia, texto, tamanho);
	copia[tamanho] = '\0';
	return copia;
}

CONSULTA_ITEM *consulta_item_criar(const char *linha){
	if (linha == NULL) return NULL;

	size_t tamanho = strcspn(linha, "\r\n");
	const char *separador = memchr(linha, '|', tamanho);
	if (separador == NULL || (size_t)(separador - linha) != 3 || memcmp(linha, "KEY", 3) != 0){
		return NULL;
	}

	const char *campo = separador + 1;
	const char *separador_valor = memchr(campo, '|', tamanho - (size_t)(campo - linha));
	if (separador_valor == NULL || separador_valor == campo) return NULL;

	const char *valor = separador_valor + 1;
	size_t tamanho_valor = tamanho - (size_t)(valor - linha);
	if (memchr(valor, '|', tamanho_valor) != NULL) return NULL;

	CONSULTA_ITEM *consulta = (CONSULTA_ITEM *)malloc(sizeof(CONSULTA_ITEM));
	if (consulta == NULL) return NULL;
	consulta->campo = duplicar_texto(campo, (size_t)(separador_valor - campo));
	consulta->valor = duplicar_texto(valor, tamanho_valor);
	if (consulta->campo == NULL || consulta->valor == NULL){
		consulta_item_apagar(&consulta);
		return NULL;
	}
	return consulta;
}

void consulta_item_apagar(CONSULTA_ITEM **consulta){
	if (consulta == NULL || *consulta == NULL) return;
	free((*consulta)->campo);
	free((*consulta)->valor);
	free(*consulta);
	*consulta = NULL;
}

const char *consulta_item_get_campo(const CONSULTA_ITEM *consulta){
	if (consulta == NULL) return NULL;
	return consulta->campo;
}

const char *consulta_item_get_valor(const CONSULTA_ITEM *consulta){
	if (consulta == NULL) return NULL;
	return consulta->valor;
}
