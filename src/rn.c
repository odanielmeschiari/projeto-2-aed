#include "lista.h"
#include "rn.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERMELHO 0
#define PRETO 1

typedef struct node {
	LISTA *lista;
	int cor;
	char *chave;
	struct node *esq;
	struct node *dir;
} NODE;

struct rn {
	char *criterio;
	NODE *raiz;
};

static char *duplicar_str(const char *s) {
	if (s == NULL) return NULL;
	size_t tam = strlen(s) + 1;
	char *copia = (char *)malloc(tam);
	if (copia != NULL) {
		memcpy(copia, s, tam);
	}
	return copia;
}

static NODE *node_criar(const char *chave, long long offset, int cor) {
	if (chave == NULL || offset < 0) return NULL;
	NODE *novo = (NODE *)malloc(sizeof(NODE));
	if (novo == NULL) return NULL;

	novo->chave = duplicar_str(chave);
	novo->lista = lista_criar();
	novo->cor = cor;
	novo->esq = NULL;
	novo->dir = NULL;

	if (novo->chave == NULL || novo->lista == NULL || !lista_inserir(novo->lista, offset)) {
		free(novo->chave);
		lista_apagar(&novo->lista);
		free(novo);
		return NULL;
	}
	return novo;
}

static bool eh_numero(const char *texto, double *saida) {
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

static int comparar_chaves(const char *a, const char *b) {
	double num_a = 0.0, num_b = 0.0;
	if (eh_numero(a, &num_a) && eh_numero(b, &num_b)) {
		if (num_a < num_b) return -1;
		if (num_a > num_b) return 1;
		return 0;
	}
	int cmp = strcmp(a, b);
	if (cmp < 0) return -1;
	if (cmp > 0) return 1;
	return 0;
}

static void node_apagar(NODE **x) {
	if (x == NULL || *x == NULL) return;
	node_apagar(&(*x)->esq);
	node_apagar(&(*x)->dir);
	lista_apagar(&(*x)->lista);
	free((*x)->chave);
	free(*x);
	*x = NULL;
}

static bool vermelho(NODE *x) {
	if (x == NULL) return false;
	return (x->cor == VERMELHO);
}

static NODE *rotacao_direita(NODE *x) {
	if (x == NULL || x->esq == NULL) return NULL;
	NODE *A = x;
	NODE *B = x->esq;
	NODE *T = x->esq->dir;

	B->dir = A;
	A->esq = T;

	B->cor = A->cor;
	A->cor = VERMELHO;
	return B;
}

static NODE *rotacao_esquerda(NODE *x) {
	if (x == NULL || x->dir == NULL) return NULL;
	NODE *A = x;
	NODE *B = x->dir;
	NODE *T = x->dir->esq;

	B->esq = A;
	A->dir = T;

	B->cor = A->cor;
	A->cor = VERMELHO;
	return B;
}

static void inverter_cores(NODE *h) {
	if (h == NULL) return;
	h->cor = !h->cor;
	if (h->esq != NULL) h->esq->cor = !h->esq->cor;
	if (h->dir != NULL) h->dir->cor = !h->dir->cor;
}

static NODE *node_inserir(NODE *raiz, const char *chave, long long offset, bool *ok) {
	if (raiz == NULL) {
		NODE *novo = node_criar(chave, offset, VERMELHO);
		if (novo == NULL) *ok = false;
		return novo;
	}

	int cmp = comparar_chaves(chave, raiz->chave);
	if (cmp < 0) {
		raiz->esq = node_inserir(raiz->esq, chave, offset, ok);
	} else if (cmp > 0) {
		raiz->dir = node_inserir(raiz->dir, chave, offset, ok);
	} else {
		*ok = lista_inserir(raiz->lista, offset);
		return raiz;
	}

	if (!(*ok)) return raiz;

	if (!vermelho(raiz->esq) && vermelho(raiz->dir)) {
		raiz = rotacao_esquerda(raiz);
	}
	if (vermelho(raiz->esq) && vermelho(raiz->esq->esq)) {
		raiz = rotacao_direita(raiz);
	}
	if (vermelho(raiz->esq) && vermelho(raiz->dir)) {
		inverter_cores(raiz);
	}

	return raiz;
}

static int max(int a, int b) {
	return (a > b) ? a : b;
}

static int node_altura(NODE *x) {
	if (x == NULL) return -1;
	int alt_esq = 1 + node_altura(x->esq);
	int alt_dir = 1 + node_altura(x->dir);
	return max(alt_esq, alt_dir);
}

static int num_nodes(NODE *x) {
	if (x == NULL) return 0;
	return 1 + num_nodes(x->esq) + num_nodes(x->dir);
}

RN *rn_criar(const char *criterio) {
	if (criterio == NULL) return NULL;
	RN *nova = (RN *)malloc(sizeof(RN));
	if (nova == NULL) return NULL;

	nova->criterio = duplicar_str(criterio);
	if (nova->criterio == NULL) {
		free(nova);
		return NULL;
	}
	nova->raiz = NULL;
	return nova;
}

void rn_apagar(RN **x) {
	if (x == NULL || *x == NULL) return;
	node_apagar(&(*x)->raiz);
	free((*x)->criterio);
	free(*x);
	*x = NULL;
}

bool rn_inserir(RN *x, const char *chave, long long offset) {
	if (x == NULL || chave == NULL || offset < 0) return false;
	bool ok = true;
	x->raiz = node_inserir(x->raiz, chave, offset, &ok);
	if (x->raiz != NULL) x->raiz->cor = PRETO;
	return ok;
}

LISTA *rn_busca(RN *x, const char *chave) {
	if (x == NULL || chave == NULL) return NULL;
	NODE *caminhante = x->raiz;
	while (caminhante != NULL) {
		int cmp = comparar_chaves(chave, caminhante->chave);
		if (cmp < 0) {
			caminhante = caminhante->esq;
		} else if (cmp > 0) {
			caminhante = caminhante->dir;
		} else {
			return caminhante->lista;
		}
	}
	return NULL;
}

int rn_altura(RN *x) {
	if (x == NULL) return -1;
	return node_altura(x->raiz);
}

int rn_num_chaves(RN *x) {
	if (x == NULL) return -1;
	return num_nodes(x->raiz);
}