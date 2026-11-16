// implementacao da tad item para a base da nasa

#include "item.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define IGUAL 0
#define MAIOR 1 // a maior que b
#define MENOR 2 // a menor que b

// campos internos do item
struct item{
	double distance_au;
	double velocity_km_s;
	double risk_score;
	double sentry_diameter_km;
	int year;
	int month;
	int days_until_approach;
	int panic_level;
	float absolute_magnitude;
	float sentry_torino_scale;
	float sentry_palermo_scale;
	char asteroid_designation[32];
	char asteroid_fullname[64];
	char close_approach_date[32];
	char threat_category[32];
	char panic_verdict[128];
	char sentry_impact_prob[32];
	bool is_past_event;
	bool is_future_event;
	bool on_sentry_list;
	float similarity;
	char *linha; // linha original do arquivo (usada na saida)
};

// le os campos da linha nasa e conserva a linha original para a exportacao
ITEM *criar_item(const char *linha_original){
	ITEM *novo = (ITEM *)malloc(sizeof(ITEM));
	if (novo == NULL) return NULL;
	novo->linha = NULL;
	// copia da linha com virgulas extras no final: campos ausentes ficam vazios
	// e a leitura nunca ultrapassa o fim da string
	size_t tam_linha = strlen(linha_original);
	char *line = (char *)malloc(tam_linha + 25);
	if (line == NULL){
		free(novo);
		return NULL;
	}
	memcpy(line, linha_original, tam_linha);
	memcpy(line + tam_linha, ",,,,,,,,,,,,,,,,,,,,,,,,", 25); // 24 virgulas + terminador
	novo->similarity = 0;
	// iniciar da primeira posicao
	int i = 0;
	int cont = 0;
	// ler cada campo e salvar no item
	// ler designacao
	cont = 0;
	while (line[i] != ','){
		novo->asteroid_designation[cont++] = line[i++];
	}
	i++;
	novo->asteroid_designation[cont] = '\0';
	// ler fullname
	// saltar espacos
	while (line[i] == ' ')
		i++;
	cont = 0;
	while (line[i] != ','){
		novo->asteroid_fullname[cont++] = line[i++];
	}
	i++;
	novo->asteroid_fullname[cont] = '\0';
	// ler approach date
	cont = 0;
	while (line[i] != ','){
		novo->close_approach_date[cont++] = line[i++];
	}
	i++;
	novo->close_approach_date[cont] = '\0';
	// ler ano
	char year[32];
	cont = 0;
	while (line[i] != ','){
		year[cont++] = line[i++];
	}
	year[cont] = '\0';
	i++;
	novo->year = atoi(year);
	// ler mes
	char mes[32];
	cont = 0;
	while (line[i] != ','){
		mes[cont++] = line[i++];
	}
	i++;
	mes[cont] = '\0';
	novo->month = atoi(mes);
	// ler distance
	char distance[32];
	cont = 0;
	while (line[i] != ','){
		distance[cont++] = line[i++];
	}
	i++;
	distance[cont] = '\0';
	novo->distance_au = atof(distance);
	// ler velocity
	char velocity[32];
	cont = 0;
	while (line[i] != ','){
		velocity[cont++] = line[i++];
	}
	i++;
	velocity[cont] = '\0';
	novo->velocity_km_s = atof(velocity);
	// ler magnitude
	char magnitude[32];
	cont = 0;
	while (line[i] != ','){
		magnitude[cont++] = line[i++];
	}
	i++;
	magnitude[cont] = '\0';
	novo->absolute_magnitude = atof(magnitude);
	// ler days_until
	char signal = (line[i] == '-') ? ('-') : '+';
	if (signal == '-') i++;
	char days[32];
	cont = 0;
	while (line[i] != ','){
		days[cont++] = line[i++];
	}
	i++;
	days[cont] = '\0';
	novo->days_until_approach = (signal == '-') ? ((-1)*atoi(days)) : (atoi(days));
	// ler past event
	char past[32];
	cont = 0;
	while (line[i] != ','){
		past[cont++] = line[i++];
	}
	i++;
	past[cont] = '\0';
	novo->is_past_event = (past[0] == 'T') ? true : false;
	// ler future event
	char future[32];
	cont = 0;
	while (line[i] != ','){
		future[cont++] = line[i++];
	}
	i++;
	future[cont] = '\0';
	novo->is_future_event = (future[0] == 'T') ? true : false;
	// ler risk score
	char risk[32];
	cont = 0;
	while (line[i] != ','){
		risk[cont++] = line[i++];
	}
	i++;
	risk[cont] = '\0';
	novo->risk_score = atof(risk);
	// ler panic level
	char panic[32];
	cont = 0;
	while (line[i] != ','){
		panic[cont++] = line[i++];
	}
	i++;
	panic[cont] = '\0';
	novo->panic_level = atoi(panic);
	// ler categoria
	cont = 0;
	while (line[i] != ','){
		novo->threat_category[cont++] = line[i++];
	}
	i++;
	novo->threat_category[cont] = '\0';
	// ler veredito (pode estar entre aspas e conter virgulas)
	cont = 0;
	if (line[i] == '"' && strchr(line + i + 1, '"') != NULL){
		i++;
		while (line[i] != '"'){
			if (cont < (int)sizeof(novo->panic_verdict) - 1) novo->panic_verdict[cont++] = line[i];
			i++;
		}
		i++; // aspas de fechamento
		while (line[i] != ',') i++;
	}
	else{
		while (line[i] != ','){
			novo->panic_verdict[cont++] = line[i++];
		}
	}
	i++;
	novo->panic_verdict[cont] = '\0';
	// ler on sentry list
	char on_sentry[32];
	cont = 0;
	while (line[i] != ','){
		on_sentry[cont++] = line[i++];
	}
	i++;
	on_sentry[cont] = '\0';
	novo->on_sentry_list = (on_sentry[0] == 'T') ? true : false;
	if (novo->on_sentry_list == true){
		// ler o que faltou
		cont = 0;
		while (line[i] != ','){
			novo->sentry_impact_prob[cont++] = line[i++];
		}
		i++;
		novo->sentry_impact_prob[cont] = '\0';
		char torino[32];
		cont = 0;
		while (line[i] != ','){
			torino[cont++] = line[i++];
		}
		i++;
		torino[cont] = '\0';
		novo->sentry_torino_scale = atof(torino);
		char signall = (line[i] == '-') ? '-' : '+';
		if (signall == '-') i++;
		char palermo[32];
		cont = 0;
		while (line[i] != ','){
			palermo[cont++] = line[i++];
		}
		i++;
		palermo[cont] = '\0';
		novo->sentry_palermo_scale = (signall == '-') ? ((-1)*atof(palermo)) : (atof(palermo));
		char diameter[32];
		cont = 0;
		while (line[i] != '\n' && line[i] != '\0' && line[i] != '\r' && line[i] != ','){
			diameter[cont++] = line[i++];
		}
		diameter[cont] = '\0';
		novo->sentry_diameter_km = atof(diameter);
	}
	else{
		// inicializar as outras variaveis como nulas
		novo->sentry_impact_prob[0] = '\0';
		novo->sentry_torino_scale = 0;
		novo->sentry_palermo_scale = 0;
		novo->sentry_diameter_km = 0;
	}
	free(line);
	// guardar a linha original (sem quebra de linha) para gravar a saida identica a entrada
	size_t tam_raw = strcspn(linha_original, "\r\n");
	novo->linha = (char *)malloc(tam_raw + 1);
	if (novo->linha != NULL){
		memcpy(novo->linha, linha_original, tam_raw);
		novo->linha[tam_raw] = '\0';
	}
	return novo;
}

// libera a linha original e a estrutura do item; depois zera o ponteiro
void item_apagar(ITEM **item){
	if (item == NULL || *item == NULL) return;
	free((*item)->linha);
	free(*item);
	*item = NULL;
}

// devolve a designacao do asteroide ou null se o item nao existir
const char *item_get_asteroid_designation(ITEM *item){
	if (item == NULL) return NULL;
	return item->asteroid_designation;
}

// devolve o nome completo do asteroide ou null se o item nao existir
const char *item_get_asteroid_fullname(ITEM *item){
	if (item == NULL) return NULL;
	return item->asteroid_fullname;
}

// devolve a data de aproximacao ou null se o item nao existir
const char *item_get_close_approach_date(ITEM *item){
	if (item == NULL) return NULL;
	return item->close_approach_date;
}

// devolve a categoria de ameaca ou null se o item nao existir
const char *item_get_threat_category(ITEM *item){
	if (item == NULL) return NULL;
	return item->threat_category;
}

// devolve o veredito de panico ou null se o item nao existir
const char *item_get_panic_verdict(ITEM *item){
	if (item == NULL) return NULL;
	return item->panic_verdict;
}

// devolve a probabilidade de impacto ou null se o item nao estiver na lista sentry
const char *item_get_sentry_impact_prob(ITEM *item){
	if (item == NULL || item->on_sentry_list == false) return NULL;
	return item->sentry_impact_prob;
}



// devolve o ano da aproximacao ou -1 se o item nao existir
int item_get_year(ITEM *item){
	if (item == NULL) return -1;
	return item->year;
}

// devolve o mes da aproximacao ou -1 se o item nao existir
int item_get_month(ITEM *item){
	if (item == NULL) return -1;
	return item->month;
}

// devolve a quantidade de dias ate a aproximacao; valores negativos indicam eventos passados
int item_get_days_until_approach(ITEM *item){
	return item->days_until_approach;
}

// devolve o nivel de panico ou -1 se o item nao existir
int item_get_panic_level(ITEM *item){
	if (item == NULL) return -1;
	return item->panic_level;
}

// devolve a distancia em unidades astronomicas ou -1 se o item nao existir
double item_get_distance_au(ITEM *item){
	if (item == NULL) return -1;
	return item->distance_au;
}

// devolve a velocidade em km/s ou -1 se o item nao existir
double item_get_velocity_km_s(ITEM *item){
	if (item == NULL) return -1;
	return item->velocity_km_s;
}

// devolve o indice de risco ou -1 se o item nao existir
double item_get_risk_score(ITEM *item){
	if (item == NULL) return -1;
	return item->risk_score;
}

// devolve o diametro sentry ou -1 se o item nao estiver na lista sentry
double item_get_sentry_diameter_km(ITEM *item){
	if (item == NULL || item->on_sentry_list == false) return -1;
	return item->sentry_diameter_km;
}

// devolve a magnitude absoluta ou -1 se o item nao existir
float item_get_absolute_magnitude(ITEM *item){
	if (item == NULL) return -1;
	return item->absolute_magnitude;
}

// devolve a escala torino do registro
float item_get_sentry_torino_scale(ITEM *item){
	return item->sentry_torino_scale;
}

// devolve a escala palermo do registro
float item_get_sentry_palermo_scale(ITEM *item){
	return item->sentry_palermo_scale;
}

// informa se a aproximacao ja ocorreu; um item nulo resulta em false
bool item_get_is_past_event(ITEM *item){
	if (item == NULL) return false;
	return item->is_past_event;
}

// informa se o asteroide esta na lista sentry; um item nulo resulta em false
bool item_get_on_sentry_list(ITEM *item){
	if (item == NULL) return false;
	return item->on_sentry_list;
}

// informa se a aproximacao ainda ocorrera; um item nulo resulta em false
bool item_get_is_future_event(ITEM *item){
	if (item == NULL) return false;
	return item->is_future_event;
}

// escreve a linha original; se ela nao estiver disponivel, monta uma linha com os campos
void item_to_line(const ITEM *item, char *string){
	// se a linha original existe, a saida e identica a entrada
	if (item->linha != NULL){
		size_t n = strlen(item->linha);
		memcpy(string, item->linha, n);
		string[n] = '\n';
		string[n + 1] = '\0';
		return;
	}
	// inicializar o buffer temporario
	char buffer[51];
	// iniciar o contador
	int i = 0;
	// salvar designacao
	int cont = 0;
	while (item->asteroid_designation[cont] != '\0'){
		string[i++] = item->asteroid_designation[cont++];
	}
	string[i++] = ',';
	// salvar fullname
	cont = 0;
	while (item->asteroid_fullname[cont] != '\0'){
		string[i++] = item->asteroid_fullname[cont++];
	}
	string[i++] = ',';
	// salvar close approach
	cont = 0;
	while (item->close_approach_date[cont] != '\0'){
		string[i++] = item->close_approach_date[cont++];
	}
	string[i++] = ',';
	// salvar year
	sprintf(buffer,"%d",item->year);
	cont = 0;
	while (buffer[cont] != '\0'){
		string[i++] = buffer[cont++];
	}
	string[i++] = ',';
	// salvar month
	sprintf(buffer,"%d",item->month);
	cont = 0;
	while (buffer[cont] != '\0'){
		string[i++] = buffer[cont++];
	}
	string[i++] = ',';
	// salvar distance
	sprintf(buffer,"%lf",item->distance_au);
	cont = 0;
	while (buffer[cont] != '\0'){
		string[i++] = buffer[cont++];
	}
	string[i++] = ',';
	// salvar velocity
	sprintf(buffer,"%lf",item->velocity_km_s);
	cont = 0;
	while (buffer[cont] != '\0'){
		string[i++] = buffer[cont++];
	}
	string[i++] = ',';
	// salvar magnitude
	sprintf(buffer,"%f",item->absolute_magnitude);
	cont = 0;
	while (buffer[cont] != '\0'){
		string[i++] = buffer[cont++];
	}
	string[i++] = ',';
	// salvar days until
	sprintf(buffer,"%d",item->days_until_approach);
	cont = 0;
	while (buffer[cont] != '\0'){
		string[i++] = buffer[cont++];
	}
	string[i++] = ',';
	// salvar is past event
	if (item->is_past_event == true){
		string[i++] = 'T';
		string[i++] = 'r';
		string[i++] = 'u';
		string[i++] = 'e';
	}
	else{
		string[i++] = 'F';
		string[i++] = 'a';
		string[i++] = 'l';
		string[i++] = 's';
		string[i++] = 'e';
	}
	string[i++] = ',';
	// salvar is future event
	if (item->is_future_event == true){
		string[i++] = 'T';
		string[i++] = 'r';
		string[i++] = 'u';
		string[i++] = 'e';
	}
	else{
		string[i++] = 'F';
		string[i++] = 'a';
		string[i++] = 'l';
		string[i++] = 's';
		string[i++] = 'e';
	}
	string[i++] = ',';
	// salvar risk
	sprintf(buffer,"%lf",item->risk_score);
	cont = 0;
	while (buffer[cont] != '\0'){
		string[i++] = buffer[cont++];
	}
	string[i++] = ',';
	// salvar panic
	sprintf(buffer,"%d",item->panic_level);
	cont = 0;
	while (buffer[cont] != '\0'){
		string[i++] = buffer[cont++];
	}
	string[i++] = ',';
	// salvar categoria
	cont = 0;
	while (item->threat_category[cont] != '\0'){
		string[i++] = item->threat_category[cont++];
	}
	string[i++] = ',';
	// salvar verdict
	cont = 0;
	while (item->panic_verdict[cont] != '\0'){
		string[i++] = item->panic_verdict[cont++];
	}
	string[i++] = ',';
	// salvar on sentry
	if (item->on_sentry_list == true){
		string[i++] = 'T';
		string[i++] = 'r';
		string[i++] = 'u';
		string[i++] = 'e';
		string[i++] = ',';
		// salvar impact
		cont = 0;
		while (item->sentry_impact_prob[cont] != '\0'){
			string[i++] = item->sentry_impact_prob[cont++];
		}
		string[i++] = ',';
		// salvar torino
		sprintf(buffer,"%.2f",item->sentry_torino_scale);
		cont = 0;
		while (buffer[cont] != '\0'){
			string[i++] = buffer[cont++];
		}
		string[i++] = ',';
		// savlar palermo
		sprintf(buffer,"%.2f",item->sentry_palermo_scale);
		cont = 0;
		while (buffer[cont] != '\0'){
			string[i++] = buffer[cont++];
		}
		string[i++] = ',';
		// savlar diameter
		sprintf(buffer,"%f",item->sentry_diameter_km);
		cont = 0;
		while (buffer[cont] != '\0'){
			string[i++] = buffer[cont++];
		}
	}
	else{
		string[i++] = 'F';
		string[i++] = 'a';
		string[i++] = 'l';
		string[i++] = 's';
		string[i++] = 'e';
		string[i++] = ',';
		string[i++] = ',';
		string[i++] = ',';
		string[i++] = ',';
	}
	// colocar terminador
	string[i++] = '\n';
	string[i] = '\0';
}

// duplica o item e sua linha original para que a copia tenha ciclo de vida independente
ITEM *item_clonar(const ITEM *item){
	if (item == NULL) return NULL;
	ITEM *copia = (ITEM *)malloc(sizeof(ITEM));
	if (copia == NULL) return NULL;
	memcpy(copia, item, sizeof(ITEM));
	copia->linha = NULL;
	if (item->linha != NULL){
		size_t tam = strlen(item->linha) + 1;
		copia->linha = (char *)malloc(tam);
		if (copia->linha == NULL){
			free(copia);
			return NULL;
		}
		memcpy(copia->linha, item->linha, tam);
	}
	return copia;
}

// converte um campo reconhecido para texto, respeitando o tamanho do buffer de saida
bool item_get_campo_texto(const ITEM *item, const char *campo, char *saida, size_t tam){
	if (item == NULL || campo == NULL || saida == NULL || tam == 0) return false;

	if (!strcmp(campo, "asteroid_designation")) snprintf(saida, tam, "%s", item->asteroid_designation);
	else if (!strcmp(campo, "asteroid_fullname")) snprintf(saida, tam, "%s", item->asteroid_fullname);
	else if (!strcmp(campo, "close_approach_date")) snprintf(saida, tam, "%s", item->close_approach_date);
	else if (!strcmp(campo, "distance_au")) snprintf(saida, tam, "%.17g", item->distance_au);
	else if (!strcmp(campo, "velocity_km_s")) snprintf(saida, tam, "%.17g", item->velocity_km_s);
	else if (!strcmp(campo, "absolute_magnitude")) snprintf(saida, tam, "%.9g", item->absolute_magnitude);
	else if (!strcmp(campo, "days_until_approach")) snprintf(saida, tam, "%d", item->days_until_approach);
	else if (!strcmp(campo, "is_past_event")) snprintf(saida, tam, "%s", item->is_past_event ? "True" : "False");
	else if (!strcmp(campo, "is_future_event")) snprintf(saida, tam, "%s", item->is_future_event ? "True" : "False");
	else if (!strcmp(campo, "risk_score")) snprintf(saida, tam, "%.17g", item->risk_score);
	else if (!strcmp(campo, "panic_level")) snprintf(saida, tam, "%d", item->panic_level);
	else if (!strcmp(campo, "threat_category")) snprintf(saida, tam, "%s", item->threat_category);
	else if (!strcmp(campo, "panic_verdict")) snprintf(saida, tam, "%s", item->panic_verdict);
	else if (!strcmp(campo, "on_sentry_list")) snprintf(saida, tam, "%s", item->on_sentry_list ? "True" : "False");
	else if (!strcmp(campo, "sentry_impact_prob")) snprintf(saida, tam, "%s", item->sentry_impact_prob);
	else if (!strcmp(campo, "sentry_torino_scale")) snprintf(saida, tam, "%.9g", item->sentry_torino_scale);
	else if (!strcmp(campo, "sentry_palermo_scale")) snprintf(saida, tam, "%.9g", item->sentry_palermo_scale);
	else if (!strcmp(campo, "sentry_diameter_km")) snprintf(saida, tam, "%.17g", item->sentry_diameter_km);
	else if (!strcmp(campo, "similarity")) snprintf(saida, tam, "%.9g", item->similarity);
	else if (!strcmp(campo, "year")) snprintf(saida, tam, "%d", item->year);
	else if (!strcmp(campo, "month")) snprintf(saida, tam, "%d", item->month);
	else return false;

	return true;
}

// atualiza a nota de similaridade armazenada no item
void item_set_similarity(ITEM *item, float similarity){
	if (item == NULL) return;
	item->similarity = similarity;
}

// devolve a nota de similaridade ou -1 se o item nao existir
float item_get_similarity(ITEM *item){
	if (item == NULL) return -1;
	return item->similarity;
}

// devolve o endereco do campo numerico indicado, sem transferir a posse do valor
void *item_get_info(const char *criterio, const ITEM *item) {
    if (item == NULL || criterio == NULL) return NULL;

    if (!strcmp(criterio, "year")) return (void*)&(item->year);
    if (!strcmp(criterio, "month")) return (void*)&(item->month);
    if (!strcmp(criterio, "days_until_approach")) return (void*)&(item->days_until_approach);
    if (!strcmp(criterio, "panic_level")) return (void*)&(item->panic_level);
    if (!strcmp(criterio, "distance_au")) return (void*)&(item->distance_au);
    if (!strcmp(criterio, "velocity_km_s")) return (void*)&(item->velocity_km_s);
    if (!strcmp(criterio, "risk_score")) return (void*)&(item->risk_score);
    if (!strcmp(criterio, "sentry_diameter_km")) return (void*)&(item->sentry_diameter_km);
    if (!strcmp(criterio, "absolute_magnitude")) return (void*)&(item->absolute_magnitude);
    if (!strcmp(criterio, "sentry_torino_scale")) return (void*)&(item->sentry_torino_scale);
    if (!strcmp(criterio, "sentry_palermo_scale")) return (void*)&(item->sentry_palermo_scale);
    if (!strcmp(criterio, "similarity")) return (void*)&(item->similarity);
    
    return NULL;
}

// compara dois itens pelo campo indicado e devolve igual, maior, menor ou -1
int item_comp(const ITEM *a, const ITEM *b, const char *criterio){
	if (a == NULL || b == NULL || criterio == NULL) return -1;

	const char *texto_a = NULL;
	const char *texto_b = NULL;
	if (!strcmp(criterio, "asteroid_designation")){
		texto_a = a->asteroid_designation;
		texto_b = b->asteroid_designation;
	}
	else if (!strcmp(criterio, "asteroid_fullname")){
		texto_a = a->asteroid_fullname;
		texto_b = b->asteroid_fullname;
	}
	else if (!strcmp(criterio, "close_approach_date")){
		texto_a = a->close_approach_date;
		texto_b = b->close_approach_date;
	}
	else if (!strcmp(criterio, "threat_category")){
		texto_a = a->threat_category;
		texto_b = b->threat_category;
	}
	else if (!strcmp(criterio, "panic_verdict")){
		texto_a = a->panic_verdict;
		texto_b = b->panic_verdict;
	}
	else if (!strcmp(criterio, "sentry_impact_prob")){
		texto_a = a->sentry_impact_prob;
		texto_b = b->sentry_impact_prob;
	}
	if (texto_a != NULL){
		int comparacao = strcmp(texto_a, texto_b);
		if (comparacao == 0) return IGUAL;
		return comparacao > 0 ? MAIOR : MENOR;
	}

	if (!strcmp(criterio, "is_past_event")){
		if (a->is_past_event == b->is_past_event) return IGUAL;
		return a->is_past_event ? MAIOR : MENOR;
	}
	if (!strcmp(criterio, "is_future_event")){
		if (a->is_future_event == b->is_future_event) return IGUAL;
		return a->is_future_event ? MAIOR : MENOR;
	}
	if (!strcmp(criterio, "on_sentry_list")){
		if (a->on_sentry_list == b->on_sentry_list) return IGUAL;
		return a->on_sentry_list ? MAIOR : MENOR;
	}

	if (!strcmp(criterio, "year")){
		if (a->year == b->year) return IGUAL;
		else if (a->year > b->year) return MAIOR;
		else return MENOR;
	}

	if (!strcmp(criterio, "month")){
		if (a->month == b->month) return IGUAL;
		else if (a->month > b->month) return MAIOR;
		else return MENOR;
	}

	if (!strcmp(criterio, "days_until_approach")){
		if (a->days_until_approach == b->days_until_approach) return IGUAL;
		else if (a->days_until_approach > b->days_until_approach) return MAIOR;
		else return MENOR;
	}

	if (!strcmp(criterio, "panic_level")){
		if (a->panic_level == b->panic_level) return IGUAL;
		else if (a->panic_level > b->panic_level) return MAIOR;
		else return MENOR;
	}

	if (!strcmp(criterio, "distance_au")){
		if (a->distance_au == b->distance_au) return IGUAL;
		else if (a->distance_au > b->distance_au) return MAIOR;
		else return MENOR;
	}

	if (!strcmp(criterio, "velocity_km_s")){
		if (a->velocity_km_s == b->velocity_km_s) return IGUAL;
		else if (a->velocity_km_s > b->velocity_km_s) return MAIOR;
		else return MENOR;
	}

	if (!strcmp(criterio, "risk_score")){
		if (a->risk_score == b->risk_score) return IGUAL;
		else if (a->risk_score > b->risk_score) return MAIOR;
		else return MENOR;
	}

	if (!strcmp(criterio, "sentry_diameter_km")){
		if (a->sentry_diameter_km == b->sentry_diameter_km) return IGUAL;
		else if (a->sentry_diameter_km > b->sentry_diameter_km) return MAIOR;
		else return MENOR;
	}

	if (!strcmp(criterio, "absolute_magnitude")){
		if (a->absolute_magnitude == b->absolute_magnitude) return IGUAL;
		else if (a->absolute_magnitude > b->absolute_magnitude) return MAIOR;
		else return MENOR;
	}

	if (!strcmp(criterio, "sentry_torino_scale")){
		if (a->sentry_torino_scale == b->sentry_torino_scale) return IGUAL;
		else if (a->sentry_torino_scale > b->sentry_torino_scale) return MAIOR;
		else return MENOR;
	}

	if (!strcmp(criterio, "sentry_palermo_scale")){
		if (a->sentry_palermo_scale == b->sentry_palermo_scale) return IGUAL;
		else if (a->sentry_palermo_scale > b->sentry_palermo_scale) return MAIOR;
		else return MENOR;
	}

	if (!strcmp(criterio, "similarity")){
		if (a->similarity == b->similarity) return IGUAL;
		else if (a->similarity > b->similarity) return MAIOR;
		else return MENOR;
	}

	return -1; // criterio desconhecido
}