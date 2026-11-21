#include "abb.h"
#include "lista.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
	LISTA *lista;
	char *chave;
	struct node *esq;
	struct node *dir;
} NODE;

struct abb {
	char *criterio;
	NODE *raiz;
};

// funcoes estaticas

static char *duplicar_str(const char *s) {
	if (s == NULL) return NULL;
	size_t tam = strlen(s) + 1;
	char *copia = (char *)malloc(tam);
	if (copia != NULL) {
		memcpy(copia, s, tam);
	}
	return copia;
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

static NODE *node_criar(const char *chave, long long offset) {
	if (chave == NULL || offset < 0) return NULL;
	NODE *novo = (NODE *)malloc(sizeof(NODE));
	if (novo == NULL) return NULL;

	novo->chave = duplicar_str(chave);
	novo->lista = lista_criar();
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

static void node_apagar(NODE **x) {
	if (x == NULL || *x == NULL) return;
	node_apagar(&(*x)->esq);
	node_apagar(&(*x)->dir);
	lista_apagar(&(*x)->lista);
	free((*x)->chave);
	free(*x);
	*x = NULL;
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

static void node_salvar(NODE *x, FILE *f) {
	if (x == NULL) {
		fprintf(f, "{NULL}\n");
		return;
	}
	fprintf(f, "{[%s]:", x->chave);
	lista_salvar_indice(x->lista, f);
	fprintf(f, "}\n");
	node_salvar(x->esq, f);
	node_salvar(x->dir, f);
}

static NODE *node_carregar(FILE *f) {
	int c = fgetc(f);
	while (isspace(c)) c = fgetc(f);
	if (c != '{') return NULL;

	c = fgetc(f);
	if (c == 'N') {
		fscanf(f, "ULL}");
		return NULL;
	}
	if (c != '[') return NULL;

	char chave[1024];
	if (fscanf(f, "%1023[^]]]:", chave) != 1) return NULL;

	NODE *novo = (NODE *)malloc(sizeof(NODE));
	if (novo == NULL) return NULL;

	novo->chave = duplicar_str(chave);
	novo->lista = lista_carregar_indice(f);
	if (novo->chave == NULL || novo->lista == NULL) {
		free(novo->chave);
		lista_apagar(&novo->lista);
		free(novo);
		return NULL;
	}

	while ((c = fgetc(f)) != EOF && c != '}');

	novo->esq = node_carregar(f);
	novo->dir = node_carregar(f);
	return novo;
}

// funcoes da interface principal

ABB *abb_criar(const char *criterio) {
	if (criterio == NULL) return NULL;
	ABB *nova = (ABB *)malloc(sizeof(ABB));
	if (nova == NULL) return NULL;

	nova->criterio = duplicar_str(criterio);
	if (nova->criterio == NULL) {
		free(nova);
		return NULL;
	}
	nova->raiz = NULL;
	return nova;
}

void abb_apagar(ABB **x) {
	if (x == NULL || *x == NULL) return;
	node_apagar(&(*x)->raiz);
	free((*x)->criterio);
	free(*x);
	*x = NULL;
}

bool abb_inserir(ABB *x, const char *chave, long long offset) {
	if (x == NULL || chave == NULL || offset < 0) return false;

	NODE *caminhante = x->raiz;
	NODE *pai = NULL;
	int cmp = 0;

	while (caminhante != NULL) {
		cmp = comparar_chaves(chave, caminhante->chave);
		if (cmp < 0) {
			pai = caminhante;
			caminhante = caminhante->esq;
		} else if (cmp > 0) {
			pai = caminhante;
			caminhante = caminhante->dir;
		} else {
			return lista_inserir(caminhante->lista, offset);
		}
	}

	NODE *novo = node_criar(chave, offset);
	if (novo == NULL) return false;

	if (pai == NULL) {
		x->raiz = novo;
	} else if (cmp < 0) {
		pai->esq = novo;
	} else {
		pai->dir = novo;
	}
	return true;
}

LISTA *abb_busca(ABB *x, const char *chave) {
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

int abb_altura(ABB *x) {
	if (x == NULL) return -1;
	return node_altura(x->raiz);
}

int abb_num_chaves(ABB *x) {
	if (x == NULL) return -1;
	return num_nodes(x->raiz);
}

bool abb_salvar_indice(ABB *x, FILE *f) {
	if (x == NULL || f == NULL) return false;

	const char *tipo_str = "INDEFINIDO";
	if (x->raiz != NULL) {
		tipo_str = eh_numero(x->raiz->chave, NULL) ? "NUMERICO" : "STRING";
	}

	fprintf(f, "Árvore: [ABB]\n");
	fprintf(f, "Tipo da chave: [%s]\n", tipo_str);
	fprintf(f, "Criterio: [%s]\n", x->criterio);
	node_salvar(x->raiz, f);
	fprintf(f, "FIM\n");

	return ferror(f) == 0;
}

ABB *abb_recuperar_indice(FILE *f) {
	if (f == NULL) return NULL;

	char linha[1024];
	char criterio[512] = "";

	while (fgets(linha, sizeof(linha), f) != NULL) {
		linha[strcspn(linha, "\r\n")] = '\0';
		if (sscanf(linha, "Criterio: [%511[^]]]", criterio) == 1) {
			break;
		}
	}

	if (criterio[0] == '\0') return NULL;

	ABB *nova = abb_criar(criterio);
	if (nova == NULL) return NULL;

	nova->raiz = node_carregar(f);
	fgets(linha, sizeof(linha), f);

	return nova;
}