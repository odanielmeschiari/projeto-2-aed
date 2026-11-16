// implementacao do item para a base da nasa

#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include <stddef.h>

// encapsulamento da linha
typedef struct item ITEM;

#define IGUAL 0
#define MAIOR 1 // a maior que b
#define MENOR 2 // a menor que b

// cria um item nasa a partir de uma linha do arquivo; retorna null se falhar
ITEM *criar_item(const char *line);
// libera o item e zera o ponteiro recebido
void item_apagar(ITEM **item);

// getters textuais: devolvem o campo armazenado ou null se item for null
const char *item_get_asteroid_designation(ITEM *item);
const char *item_get_asteroid_fullname(ITEM *item);
const char *item_get_close_approach_date(ITEM *item);
const char *item_get_threat_category(ITEM *item);
const char *item_get_panic_verdict(ITEM *item);
// devolve null tambem quando o asteroide nao consta na lista sentry
const char *item_get_sentry_impact_prob(ITEM *item);

// getters inteiros dos campos de data e nivel de panico
int item_get_year(ITEM *item);
int item_get_month(ITEM *item);
int item_get_days_until_approach(ITEM *item);
int item_get_panic_level(ITEM *item);

// getters numericos: devolvem -1 se item for null
double item_get_distance_au(ITEM *item);
double item_get_velocity_km_s(ITEM *item);
double item_get_risk_score(ITEM *item);
// devolve -1 se item for null ou nao constar na lista sentry
double item_get_sentry_diameter_km(ITEM *item);

// getters de campos float; a magnitude absoluta devolve -1 se item for null
float item_get_absolute_magnitude(ITEM *item);
float item_get_sentry_torino_scale(ITEM *item);
float item_get_sentry_palermo_scale(ITEM *item);

// getters booleanos; um ponteiro nulo e tratado como false
bool item_get_is_past_event(ITEM *item);
bool item_get_is_future_event(ITEM *item);
bool item_get_on_sentry_list(ITEM *item);

// copia para string a linha original do item, terminada por quebra de linha
void item_to_line(const ITEM *item, char *string);

// guarda no item a similaridade calculada pela consulta
void item_set_similarity(ITEM *item, float similarity);

// devolve a similaridade armazenada ou -1 se item for null
float item_get_similarity(ITEM *item);

// cria uma copia independente; quem recebe o ponteiro deve libera-lo com item_apagar
ITEM *item_clonar(const ITEM *item);

// escreve o campo em formato textual; retorna false se o campo nao for reconhecido
bool item_get_campo_texto(const ITEM *item, const char *campo, char *saida, size_t tam);

// devolve o endereco do valor numerico associado ao criterio, ou null se nao existir
void *item_get_info(const char *criterio, const ITEM *item);

// compara os itens pelo criterio; devolve igual, maior, menor ou -1 para criterio invalido
int item_comp(const ITEM *a, const ITEM *b, const char *criterio);

#endif