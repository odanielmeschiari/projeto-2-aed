#include "consulta_generalizada.h"

#include <stdlib.h>
#include <string.h>

struct consulta_generalizada {
	char *campo;
	char *valor;
	size_t indice;
};

static char *duplicar(const char *texto, size_t tamanho){
	char *copia = (char *)malloc(tamanho + 1);
	if (copia == NULL) return NULL;
	memcpy(copia, texto, tamanho);
	copia[tamanho] = '\0';
	return copia;
}

CONSULTA_GENERALIZADA *consulta_generalizada_criar(const char *linha, const char *cabecalho){
	if (linha == NULL || cabecalho == NULL) return NULL;

	size_t tamanho = strcspn(linha, "\r\n");
	const char *primeiro = memchr(linha, '|', tamanho);
	if (primeiro == NULL || (size_t)(primeiro - linha) != 3 || memcmp(linha, "KEY", 3) != 0) return NULL;

	const char *campo = primeiro + 1;
	const char *segundo = memchr(campo, '|', tamanho - (size_t)(campo - linha));
	if (segundo == NULL || segundo == campo) return NULL;

	const char *valor = segundo + 1;
	size_t tamanho_valor = tamanho - (size_t)(valor - linha);
	if (tamanho_valor == 0 || memchr(valor, '|', tamanho_valor) != NULL) return NULL;

	char delimitador = strchr(cabecalho, ';') != NULL ? ';' : ',';
	size_t quantidade = 1;
	for (const char *p = cabecalho; *p != '\0'; p++){
		if (*p == delimitador) quantidade++;
	}

	char *cabecalho_copia = duplicar(cabecalho, strlen(cabecalho));
	if (cabecalho_copia == NULL) return NULL;

	size_t indice = quantidade;
	char *inicio = cabecalho_copia;
	size_t tamanho_campo = (size_t)(segundo - campo);
	for (size_t i = 0; i < quantidade; i++){
		char *fim = strchr(inicio, delimitador);
		if (fim != NULL) *fim = '\0';
		if (strlen(inicio) == tamanho_campo && memcmp(inicio, campo, tamanho_campo) == 0){
			indice = i;
			break;
		}
		if (fim == NULL) break;
		inicio = fim + 1;
	}
	free(cabecalho_copia);
	if (indice == quantidade) return NULL;

	CONSULTA_GENERALIZADA *consulta = (CONSULTA_GENERALIZADA *)malloc(sizeof(CONSULTA_GENERALIZADA));
	if (consulta == NULL) return NULL;
	consulta->campo = duplicar(campo, tamanho_campo);
	consulta->valor = duplicar(valor, tamanho_valor);
	consulta->indice = indice;
	if (consulta->campo == NULL || consulta->valor == NULL){
		consulta_generalizada_apagar(&consulta);
		return NULL;
	}
	return consulta;
}

void consulta_generalizada_apagar(CONSULTA_GENERALIZADA **consulta){
	if (consulta == NULL || *consulta == NULL) return;
	free((*consulta)->campo);
	free((*consulta)->valor);
	free(*consulta);
	*consulta = NULL;
}

const char *consulta_generalizada_get_campo(const CONSULTA_GENERALIZADA *consulta){
	if (consulta == NULL) return NULL;
	return consulta->campo;
}

const char *consulta_generalizada_get_valor(const CONSULTA_GENERALIZADA *consulta){
	if (consulta == NULL) return NULL;
	return consulta->valor;
}

size_t consulta_generalizada_get_indice(const CONSULTA_GENERALIZADA *consulta){
	if (consulta == NULL) return (size_t)-1;
	return consulta->indice;
}
