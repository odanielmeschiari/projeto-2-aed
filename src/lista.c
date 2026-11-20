#include "lista.h"
#include "item.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node{
	long long offset;
	struct node *next;
}NODE;

struct lista{
	NODE *head;
	NODE *fim;
	int tamanho;
};

// funcoes estaticas

static NODE *node_criar(long long offset){
	if (offset < 0) return NULL;
	NODE *novo = (NODE *)malloc(sizeof(NODE));
	if (novo != NULL){
		novo->offset = offset;
		novo->next = NULL;
	}
	return novo;
}

// funcoes da interface principal

LISTA *lista_criar(void){
	LISTA *nova = (LISTA *)malloc(sizeof(LISTA));
	if (nova != NULL){
		nova->head = NULL;
		nova->fim = NULL;
		nova->tamanho = 0;
	}
	return nova;
}

void lista_apagar(LISTA **x){
	if (x == NULL || *x == NULL) return;
	NODE *inicio = (*x)->head;
	for (int i = 0; i<(*x)->tamanho; i++){
		NODE *temp = inicio;
		inicio = inicio->next;
		free(temp);
		temp = NULL;
	}
	free(*x);
	*x = NULL;
}

bool lista_inserir(LISTA *x, long long offset){
	if (x == NULL || offset < 0) return false;
	NODE *novo = node_criar(offset);
	if (novo == NULL) return false;
	if (x->head == NULL)
		x->head = novo;
	else
		x->fim->next = novo;
	x->fim = novo;
	x->tamanho++;
	return true;
}

long long lista_get_offset(LISTA *x, long long indice){
	if (x == NULL || indice < 0 || indice >= x->tamanho) return -1;
	NODE *temp = x->head;
	long long cont = 0;
	while (temp != NULL && cont < indice){
		temp = temp->next;
		cont++;
	}
	if (temp == NULL) return -1;
	return temp->offset;
}

long long lista_tamanho(LISTA *x){
	if (x == NULL) return -1;
	return x->tamanho;
}
