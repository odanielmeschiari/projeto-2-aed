#ifndef BUSCA_H
#define BUSCA_H

#include <stdbool.h>
#include <stddef.h>

bool busca_simples(
	const char *arquivo_dados,
	const char *arquivo_consulta,
	const char *arquivo_saida,
	const char *tipo_arvore,
	void *indice,
	const char *criterio
);

bool busca_complexa(
	const char *arquivo_dados,
	const char *arquivo_consulta,
	const char *arquivo_saida,
	const char **tipos_arvore,
	void **indices,
	const char **criterios,
	size_t quantidade_indices
);

#endif
