#include "item_enem.h"

#include <stdlib.h>
#include <string.h>

struct item_enem {
	char *linha;
	char *dados;
	char **campos;
	size_t quantidade;
};

static char *duplicar_texto(const char *texto, size_t tamanho){
	char *copia = (char *)malloc(tamanho + 1);
	if (copia == NULL) return NULL;
	memcpy(copia, texto, tamanho);
	copia[tamanho] = '\0';
	return copia;
}

ITEM_ENEM *item_enem_criar(const char *linha){
	if (linha == NULL) return NULL;

	size_t tamanho = strcspn(linha, "\r\n");
	char *dados = duplicar_texto(linha, tamanho);
	char *original = duplicar_texto(linha, tamanho);
	if (dados == NULL || original == NULL){
		free(dados);
		free(original);
		return NULL;
	}

	size_t quantidade = 1;
	int entre_aspas = 0;
	for (size_t i = 0; i < tamanho; i++){
		if (dados[i] == '"'){
			if (entre_aspas && i + 1 < tamanho && dados[i + 1] == '"'){
				i++;
			}
			else{
				entre_aspas = !entre_aspas;
			}
		}
		else if (dados[i] == ';' && !entre_aspas){
			quantidade++;
		}
	}
	if (entre_aspas){
		free(dados);
		free(original);
		return NULL;
	}

	char **campos = (char **)malloc(quantidade * sizeof(char *));
	if (campos == NULL){
		free(dados);
		free(original);
		return NULL;
	}

	char *leitura = dados;
	char *escrita = dados;
	size_t indice = 0;
	while (indice < quantidade){
		campos[indice++] = escrita;
		if (*leitura == '"'){
			leitura++;
			int fechado = 0;
			while (*leitura != '\0'){
				if (*leitura == '"' && leitura[1] == '"'){
					*escrita++ = '"';
					leitura += 2;
				}
				else if (*leitura == '"'){
					leitura++;
					fechado = 1;
					break;
				}
				else{
					*escrita++ = *leitura++;
				}
			}
			if (!fechado || (*leitura != ';' && *leitura != '\0')){
				free(campos);
				free(dados);
				free(original);
				return NULL;
			}
		}
		else{
			while (*leitura != ';' && *leitura != '\0'){
				*escrita++ = *leitura++;
			}
		}

		int proximo_campo = (*leitura == ';');
		if (proximo_campo) leitura++;
		*escrita++ = '\0';
	}

	ITEM_ENEM *item = (ITEM_ENEM *)malloc(sizeof(ITEM_ENEM));
	if (item == NULL){
		free(campos);
		free(dados);
		free(original);
		return NULL;
	}
	item->linha = original;
	item->dados = dados;
	item->campos = campos;
	item->quantidade = quantidade;
	return item;
}

void item_enem_apagar(ITEM_ENEM **item){
	if (item == NULL || *item == NULL) return;
	free((*item)->linha);
	free((*item)->dados);
	free((*item)->campos);
	free(*item);
	*item = NULL;
}

size_t item_enem_numero_campos(const ITEM_ENEM *item){
	if (item == NULL) return 0;
	return item->quantidade;
}

const char *item_enem_get_campo(const ITEM_ENEM *item, size_t indice){
	if (item == NULL || indice >= item->quantidade) return NULL;
	return item->campos[indice];
}

const char *item_enem_get_linha(const ITEM_ENEM *item){
	if (item == NULL) return NULL;
	return item->linha;
}
