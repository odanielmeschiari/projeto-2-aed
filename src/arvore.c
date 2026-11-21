#include "abb.h"
#include "arvore.h"
#include "avl.h"
#include "lista.h"
#include "rn.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void *arvore_criar(const char *tipo, const char *criterio) {
	if (tipo == NULL || criterio == NULL) return NULL;
	if (!strcmp(tipo, "abb")) return (void *)abb_criar(criterio);
	else if (!strcmp(tipo, "avl")) return (void *)avl_criar(criterio);
	else if (!strcmp(tipo, "rubro_negra")) return (void *)rn_criar(criterio);
	else return NULL;
}

void arvore_apagar(const char *tipo, void **x) {
	if (tipo == NULL || x == NULL || *x == NULL) return;
	if (!strcmp(tipo, "abb")) abb_apagar((ABB **)x);
	else if (!strcmp(tipo, "avl")) avl_apagar((AVL **)x);
	else if (!strcmp(tipo, "rubro_negra")) rn_apagar((RN **)x);
}

bool arvore_inserir(const char *tipo, void *x, const char *chave, long long offset) {
	if (tipo == NULL || x == NULL || chave == NULL || offset < 0) return false;
	if (!strcmp(tipo, "abb")) return abb_inserir((ABB *)x, chave, offset);
	else if (!strcmp(tipo, "avl")) return avl_inserir((AVL *)x, chave, offset);
	else if (!strcmp(tipo, "rubro_negra")) return rn_inserir((RN *)x, chave, offset);
	else return false;
}

LISTA *arvore_busca(const char *tipo, void *x, const char *chave) {
	if (tipo == NULL || x == NULL || chave == NULL) return NULL;
	if (!strcmp(tipo, "abb")) return abb_busca((ABB *)x, chave);
	else if (!strcmp(tipo, "avl")) return avl_busca((AVL *)x, chave);
	else if (!strcmp(tipo, "rubro_negra")) return rn_busca((RN *)x, chave);
	else return NULL;
}

int arvore_altura(const char *tipo, void *x) {
	if (tipo == NULL || x == NULL) return -1;
	if (!strcmp(tipo, "abb")) return abb_altura((ABB *)x);
	else if (!strcmp(tipo, "avl")) return avl_altura((AVL *)x);
	else if (!strcmp(tipo, "rubro_negra")) return rn_altura((RN *)x);
	else return -1;
}

int arvore_num_chaves(const char *tipo, void *x) {
	if (tipo == NULL || x == NULL) return -1;
	if (!strcmp(tipo, "abb")) return abb_num_chaves((ABB *)x);
	else if (!strcmp(tipo, "avl")) return avl_num_chaves((AVL *)x);
	else if (!strcmp(tipo, "rubro_negra")) return rn_num_chaves((RN *)x);
	else return -1;
}

bool arvore_salvar_indice(const char *tipo, void *x, FILE *f) {
	if (tipo == NULL || x == NULL || f == NULL) return false;
	if (!strcmp(tipo, "abb")) return abb_salvar_indice((ABB *)x, f);
	else if (!strcmp(tipo, "avl")) return avl_salvar_indice((AVL *)x, f);
	else if (!strcmp(tipo, "rubro_negra")) return rn_salvar_indice((RN *)x, f);
	else return false;
}

void *arvore_recuperar_indice(const char *tipo, FILE *f) {
	if (tipo == NULL || f == NULL) return NULL;
	if (!strcmp(tipo, "abb")) return (void *)abb_recuperar_indice(f);
	else if (!strcmp(tipo, "avl")) return (void *)avl_recuperar_indice(f);
	else if (!strcmp(tipo, "rubro_negra")) return (void *)rn_recuperar_indice(f);
	else return NULL;
}
