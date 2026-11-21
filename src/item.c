#include "item.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct item {
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

static const char *avancar_campo(const char *cursor, char delimitador){
	int entre_aspas = 0;
	while (*cursor != '\0'){
		if (*cursor == '"'){
			if (entre_aspas && cursor[1] == '"'){
				cursor += 2;
				continue;
			}
			entre_aspas = !entre_aspas;
		} else if (*cursor == delimitador && !entre_aspas){
			return cursor;
		}
		cursor++;
	}
	return NULL;
}

static char *extrair_texto_campo(const char *inicio, size_t tamanho){
	if (tamanho >= 2 && inicio[0] == '"' && inicio[tamanho - 1] == '"'){
		inicio++;
		tamanho -= 2;
	}
	char *resultado = (char *)malloc(tamanho + 1);
	if (resultado == NULL) return NULL;

	size_t j = 0;
	for (size_t i = 0; i < tamanho; i++){
		if (inicio[i] == '"' && i + 1 < tamanho && inicio[i + 1] == '"'){
			resultado[j++] = '"';
			i++;
		} else {
			resultado[j++] = inicio[i];
		}
	}
	resultado[j] = '\0';
	return resultado;
}

static char *obter_campo(const char *linha, const char *cabecalho, const char *campo, size_t *indice){
	if (linha == NULL || cabecalho == NULL || campo == NULL){
		if (indice != NULL) *indice = (size_t)-1;
		return NULL;
	}

	char delimitador = delimitar(cabecalho);
	char *linha_copia = duplicar(linha, strcspn(linha, "\r\n"));
	if (linha_copia == NULL){
		if (indice != NULL) *indice = (size_t)-1;
		return NULL;
	}

	size_t alvo = (size_t)-1;
	size_t col_atual = 0;
	const char *inicio_coluna = cabecalho;
	while (inicio_coluna != NULL){
		const char *fim_coluna = avancar_campo(inicio_coluna, delimitador);
		size_t tam_col = fim_coluna != NULL ? (size_t)(fim_coluna - inicio_coluna) : strcspn(inicio_coluna, "\r\n");
		char *nome_col = extrair_texto_campo(inicio_coluna, tam_col);
		if (nome_col == NULL){
			free(linha_copia);
			if (indice != NULL) *indice = (size_t)-1;
			return NULL;
		}
		bool encontrou = (strcmp(nome_col, campo) == 0);
		free(nome_col);
		if (encontrou){
			alvo = col_atual;
			break;
		}
		if (fim_coluna == NULL) break;
		inicio_coluna = fim_coluna + 1;
		col_atual++;
	}

	if (alvo == (size_t)-1){
		free(linha_copia);
		if (indice != NULL) *indice = (size_t)-1;
		return NULL;
	}

	const char *inicio_valor = linha_copia;
	char *valor = NULL;
	for (size_t i = 0; i <= alvo; i++){
		const char *fim_valor = avancar_campo(inicio_valor, delimitador);
		if (i == alvo){
			size_t tam_val = fim_valor != NULL ? (size_t)(fim_valor - inicio_valor) : strlen(inicio_valor);
			valor = extrair_texto_campo(inicio_valor, tam_val);
			break;
		}
		if (fim_valor == NULL){
			valor = NULL;
			break;
		}
		inicio_valor = fim_valor + 1;
	}

	free(linha_copia);
	if (indice != NULL) *indice = alvo;
	return valor;
}

ITEM *item_criar(const char *linha, const char *cabecalho, const char *campo, long long offset){
	if (linha == NULL || campo == NULL) return NULL;

	ITEM *item = (ITEM *)malloc(sizeof(ITEM));
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
		item_apagar(&item);
		return NULL;
	}
	return item;
}

void item_apagar(ITEM **item){
	if (item == NULL || *item == NULL) return;
	free((*item)->linha);
	free((*item)->campo);
	free((*item)->valor);
	free(*item);
	*item = NULL;
}

const char *item_get_linha(const ITEM *item){
	if (item == NULL) return NULL;
	return item->linha;
}

const char *item_get_campo(const ITEM *item){
	if (item == NULL) return NULL;
	return item->campo;
}

const char *item_get_valor(const ITEM *item){
	if (item == NULL) return NULL;
	return item->valor;
}

size_t item_get_indice(const ITEM *item){
	if (item == NULL) return (size_t)-1;
	return item->indice;
}

long long item_get_offset(const ITEM *item){
	if (item == NULL) return -1;
	return item->offset;
}

static bool eh_numero(const char *texto, double *saida){
	if (texto == NULL || *texto == '\0') return false;
	while (isspace((unsigned char)*texto)) texto++;
	if (*texto == '\0') return false;

	char *fim = NULL;
	double valor = strtod(texto, &fim);
	while (isspace((unsigned char)*fim)) fim++;
	if (*fim != '\0') return false;

	if (saida != NULL) *saida = valor;
	return true;
}

int item_cmp(const ITEM *a, const ITEM *b, const char *criterio){
	if (a == NULL || b == NULL) return 0;
	(void)criterio;

	const char *val_a = a->valor != NULL ? a->valor : "";
	const char *val_b = b->valor != NULL ? b->valor : "";

	double num_a = 0.0, num_b = 0.0;
	if (eh_numero(val_a, &num_a) && eh_numero(val_b, &num_b)){
		if (num_a > num_b) return 1;
		if (num_a < num_b) return -1;
		return 0;
	}

	int cmp = strcmp(val_a, val_b);
	if (cmp > 0) return 1;
	if (cmp < 0) return -1;
	return 0;
}