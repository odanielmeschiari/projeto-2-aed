#include "busca.h"
#include "arvore.h"
#include "consulta.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_LINHA 65536

typedef struct {
	long long *offsets;
	size_t quantidade;
	size_t capacidade;
} RESULTADOS;

static void remover_quebra(char *linha) {
	if (linha == NULL) return;
	linha[strcspn(linha, "\r\n")] = '\0';
}

static void resultados_apagar(RESULTADOS *resultados) {
	if (resultados == NULL) return;
	free(resultados->offsets);
	resultados->offsets = NULL;
	resultados->quantidade = 0;
	resultados->capacidade = 0;
}

static bool resultados_adicionar(RESULTADOS *resultados, long long offset) {
	for (size_t i = 0; i < resultados->quantidade; i++) {
		if (resultados->offsets[i] == offset) return true;
	}

	if (resultados->quantidade == resultados->capacidade) {
		size_t nova_capacidade = resultados->capacidade == 0 ? 64 : resultados->capacidade * 2;
		long long *nova_lista = (long long *)realloc(
			resultados->offsets,
			nova_capacidade * sizeof(long long)
		);
		if (nova_lista == NULL) return false;
		resultados->offsets = nova_lista;
		resultados->capacidade = nova_capacidade;
	}

	resultados->offsets[resultados->quantidade] = offset;
	resultados->quantidade++;
	return true;
}

static int comparar_offset(const void *a, const void *b) {
	long long primeiro = *(const long long *)a;
	long long segundo = *(const long long *)b;
	if (primeiro < segundo) return -1;
	if (primeiro > segundo) return 1;
	return 0;
}

static bool adicionar_lista(
	RESULTADOS *resultados,
	const char *tipo_arvore,
	void *indice,
	const char *valor
) {
	LISTA *lista = arvore_busca(tipo_arvore, indice, valor);
	long long quantidade = lista_tamanho(lista);
	if (quantidade < 0) return true;

	for (long long i = 0; i < quantidade; i++) {
		long long offset = lista_get_offset(lista, i);
		if (!resultados_adicionar(resultados, offset)) return false;
	}
	return true;
}

static bool escrever_resultados(
	const char *arquivo_dados,
	const char *arquivo_saida,
	RESULTADOS *resultados
) {
	FILE *dados = fopen(arquivo_dados, "rb");
	FILE *saida = fopen(arquivo_saida, "wb");
	if (dados == NULL || saida == NULL) {
		if (dados != NULL) fclose(dados);
		if (saida != NULL) fclose(saida);
		return false;
	}

	char linha[TAMANHO_LINHA];
	if (fgets(linha, sizeof(linha), dados) == NULL) {
		fclose(dados);
		fclose(saida);
		return false;
	}
	fwrite(linha, 1, strlen(linha), saida);
	if (strchr(linha, '\n') == NULL) fputc('\n', saida);

	if (resultados->quantidade > 1) {
		qsort(
			resultados->offsets,
			resultados->quantidade,
			sizeof(long long),
			comparar_offset
		);
	}

	for (size_t i = 0; i < resultados->quantidade; i++) {
		if (fseek(dados, resultados->offsets[i], SEEK_SET) != 0) continue;
		if (fgets(linha, sizeof(linha), dados) == NULL) continue;
		fwrite(linha, 1, strlen(linha), saida);
		if (strchr(linha, '\n') == NULL) fputc('\n', saida);
	}

	fclose(dados);
	fclose(saida);
	return true;
}

static bool executar_busca(
	const char *arquivo_dados,
	const char *arquivo_consulta,
	const char *arquivo_saida,
	const char **tipos_arvore,
	void **indices,
	const char **criterios,
	size_t quantidade_indices
) {
	if (arquivo_dados == NULL || arquivo_consulta == NULL ||
		arquivo_saida == NULL || tipos_arvore == NULL ||
		indices == NULL || criterios == NULL || quantidade_indices == 0) {
		return false;
	}

	FILE *dados = fopen(arquivo_dados, "rb");
	FILE *consultas = fopen(arquivo_consulta, "r");
	if (dados == NULL || consultas == NULL) {
		if (dados != NULL) fclose(dados);
		if (consultas != NULL) fclose(consultas);
		return false;
	}

	char cabecalho[TAMANHO_LINHA];
	if (fgets(cabecalho, sizeof(cabecalho), dados) == NULL) {
		fclose(dados);
		fclose(consultas);
		return false;
	}
	remover_quebra(cabecalho);

	RESULTADOS resultados;
	resultados.offsets = NULL;
	resultados.quantidade = 0;
	resultados.capacidade = 0;

	char linha[TAMANHO_LINHA];
	bool ok = true;
	while (fgets(linha, sizeof(linha), consultas) != NULL) {
		CONSULTA *consulta = consulta_criar(linha, cabecalho);
		if (consulta == NULL) continue;

		const char *campo = consulta_get_campo(consulta);
		const char *valor = consulta_get_valor(consulta);
		for (size_t i = 0; i < quantidade_indices; i++) {
			if (strcmp(campo, criterios[i]) == 0) {
				if (!adicionar_lista(&resultados, tipos_arvore[i], indices[i], valor)) {
					ok = false;
					break;
				}
			}
		}
		consulta_apagar(&consulta);
		if (!ok) break;
	}

	fclose(dados);
	fclose(consultas);

	if (!ok) {
		resultados_apagar(&resultados);
		return false;
	}

	ok = escrever_resultados(arquivo_dados, arquivo_saida, &resultados);
	resultados_apagar(&resultados);
	return ok;
}

bool busca_simples(
	const char *arquivo_dados,
	const char *arquivo_consulta,
	const char *arquivo_saida,
	const char *tipo_arvore,
	void *indice,
	const char *criterio
) {
	const char *tipos[1];
	void *indices[1];
	const char *criterios[1];

	tipos[0] = tipo_arvore;
	indices[0] = indice;
	criterios[0] = criterio;

	return executar_busca(
		arquivo_dados,
		arquivo_consulta,
		arquivo_saida,
		tipos,
		indices,
		criterios,
		1
	);
}

bool busca_complexa(
	const char *arquivo_dados,
	const char *arquivo_consulta,
	const char *arquivo_saida,
	const char **tipos_arvore,
	void **indices,
	const char **criterios,
	size_t quantidade_indices
) {
	return executar_busca(
		arquivo_dados,
		arquivo_consulta,
		arquivo_saida,
		tipos_arvore,
		indices,
		criterios,
		quantidade_indices
	);
}
