#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "avl.h"
#include "lista.h"

typedef struct node{
	char *chave;
	int altura;
	LISTA *lista;
	struct node *esq;
	struct node *dir;
}NODE;

struct avl{
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

static NODE *node_criar(const char *chave, long long offset) {
	if (chave == NULL || offset < 0) return NULL;
	NODE *novo = (NODE *)malloc(sizeof(NODE));
	if (novo == NULL) return NULL;

	novo->chave = duplicar_str(chave);
	novo->lista = lista_criar();
	novo->altura = 0;
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

static void node_apagar(NODE **x){
	if (x == NULL || *x == NULL) return;
	node_apagar(&(*x)->esq);
	node_apagar(&(*x)->dir);
	lista_apagar(&(*x)->lista);
	free((*x)->chave);
	free(*x);
	*x = NULL;
}

static int num_nodes(NODE *x) {
	if (x == NULL) return 0;
	return 1 + num_nodes(x->esq) + num_nodes(x->dir);
}

static int max(int a, int b){
	return (a > b) ? a : b;
}

static int get_alt(NODE *raiz){
	if (raiz == NULL) return -1;
	return raiz->altura;
}

static int get_fb(NODE *n){
	if (n == NULL) return 0;
	return get_alt(n->esq) - get_alt(n->dir);
}

static NODE *rotacao_direita(NODE *raiz){
	if (raiz == NULL) return NULL;
	NODE *A = raiz;
	NODE *B = raiz->esq;
	NODE *T = B->dir;

	B->dir = A;
	A->esq = T;

	A->altura = 1 + max(get_alt(A->esq), get_alt(A->dir));
	B->altura = 1 + max(get_alt(B->esq), get_alt(B->dir));

	return B;
}

static NODE *rotacao_esquerda(NODE *raiz){
	if (raiz == NULL) return NULL;
	NODE *A = raiz;
	NODE *B = raiz->dir;
	NODE *T = B->esq;

	B->esq = A;
	A->dir = T;

	A->altura = 1 + max(get_alt(A->esq), get_alt(A->dir));
	B->altura = 1 + max(get_alt(B->esq), get_alt(B->dir));

	return B;
}

static NODE *node_inserir(NODE *raiz, const char *chave, long long offset, bool *ok) {
	if (raiz == NULL) {
		NODE *novo = node_criar(chave, offset);
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

	raiz->altura = 1 + max(get_alt(raiz->esq), get_alt(raiz->dir));
	int fb_raiz = get_fb(raiz);

	if (fb_raiz > 1 && get_fb(raiz->esq) >= 0) {
		return rotacao_direita(raiz);
	}
	if (fb_raiz > 1 && get_fb(raiz->esq) < 0) {
		raiz->esq = rotacao_esquerda(raiz->esq);
		return rotacao_direita(raiz);
	}
	if (fb_raiz < -1 && get_fb(raiz->dir) <= 0) {
		return rotacao_esquerda(raiz);
	}
	if (fb_raiz < -1 && get_fb(raiz->dir) > 0) {
		raiz->dir = rotacao_direita(raiz->dir);
		return rotacao_esquerda(raiz);
	}

	return raiz;
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
	// a altura nao eh salva no arquivo, entao eh recalculada a partir dos filhos
	novo->altura = 1 + max(get_alt(novo->esq), get_alt(novo->dir));
	return novo;
}

// funcoes da interface principal

AVL *avl_criar(const char *criterio){
	if (criterio == NULL) return NULL;
	AVL *nova = (AVL *)malloc(sizeof(AVL));
	if (nova != NULL){
		nova->criterio = duplicar_str(criterio);
		nova->raiz = NULL;
	}
	return nova;
}

void avl_apagar(AVL **x){
	if (x == NULL || *x == NULL) return;
	node_apagar(&(*x)->raiz);
	free((*x)->criterio);
	free(*x);
	*x = NULL;
}

bool avl_inserir(AVL *x, const char *chave, long long offset) {
	if (x == NULL || chave == NULL || offset < 0) return false;
	bool ok = true;
	x->raiz = node_inserir(x->raiz, chave, offset, &ok);
	return ok;
}

LISTA *avl_busca(AVL *x, const char *chave){
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

int avl_altura(AVL *x){
	if (x == NULL) return -1;
	return get_alt(x->raiz);
}

int avl_num_chaves(AVL *x){
	return num_nodes(x->raiz);
}

bool avl_salvar_indice(AVL *x, FILE *f) {
	if (x == NULL || f == NULL) return false;

	const char *tipo_str = "INDEFINIDO";
	if (x->raiz != NULL) {
		tipo_str = eh_numero(x->raiz->chave, NULL) ? "NUMERICO" : "STRING";
	}

	fprintf(f, "Árvore: [AVL]\n");
	fprintf(f, "Tipo da chave: [%s]\n", tipo_str);
	fprintf(f, "Criterio: [%s]\n", x->criterio);
	node_salvar(x->raiz, f);
	fprintf(f, "FIM\n");

	return ferror(f) == 0;
}

AVL *avl_recuperar_indice(FILE *f) {
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

	AVL *nova = avl_criar(criterio);
	if (nova == NULL) return NULL;

	nova->raiz = node_carregar(f);
	fgets(linha, sizeof(linha), f);

	return nova;
}
