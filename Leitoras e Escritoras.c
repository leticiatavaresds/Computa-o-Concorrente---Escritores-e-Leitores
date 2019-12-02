#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// criando a variável ponteiro para o arquivo
FILE *pont_arq;


int NLEI, NESCR;
int nlida, nescrita;
int dig;

int var = 8;

//nmero de leitoras lendo
int l_cont = 0;

//nmero de escritoras escrevendo (só pode no máximo 1)
int e_cont = 0;

//numero de leitoras esperando
int l_esp = 0;

//numero de escritoras esperando
int e_esp = 0;

// variável de condição para testar quando uma escritora pode escrever
pthread_cond_t pode_escr;

// variável de condição para testar quando uma leitora pode ler
pthread_cond_t podeler;

// mutex para a sincronização
pthread_mutex_t mutex;

int num_Digitos(int i){
	int contaDigitos = 0, valor;

	valor = i;
	if (valor == 0) contaDigitos = 1;
	else
	while (valor != 0)
	{
	   contaDigitos = contaDigitos + 1;
	   valor = valor / 10;
	}
  return contaDigitos;
}

int comeca_leitura(int i) {

	int leitura;
	pthread_mutex_lock(&mutex);
	nlida++;

	//Se houver alguma escritora escrevendo ou uma ou mais escritoras escrevendo, vai pra fila de leitoras
	if (e_cont == 1 || e_esp > 0) {
		// incrementa o número de leitoras esperando
		//printf("Leitora entrou na fila\n");
		fprintf(pont_arq, "\nl_entrou_fila(%d,%d)", i,var);
		l_esp++;
		// fica na fila até receber o sinal de que pode ler
		while (1) {
			pthread_cond_wait(&podeler, &mutex);
			fprintf(pont_arq, "\nl_recebeu_sinal(%d,%d)", i,var);
			if (e_cont == 0) break;
			fprintf(pont_arq, "\nl_continuou_fila(%d,%d)", i,var);
		}

		fprintf(pont_arq, "\nl_saiu_fila(%d,%d)", i,var);
		l_esp--;

	}

	printf("\nLeitora %d está lendo a variável %d", i,var);

	fprintf(arq, "\n%d",var);
	l_cont++;
	fprintf(pont_arq, "\nleitora_leu(%d,%d)", i,var);
	leitura = var;
	pthread_mutex_unlock(&mutex);
	return leitura;
		//pthread_cond_broadcast(&podeler);


};

void termina_leitura(int i)
{



	pthread_mutex_lock(&mutex);

	//Informa qe parou de ler (--l_cont)
	//Se não houver mais leitoras lendo, libera as escritoras

	fprintf(pont_arq, "\ntermina_leitura(%d,%d)", i,var);
	if (--l_cont == 0){
		fprintf(pont_arq, "\nl_manda_sinal_l(%d,%d)", i,var);
		pthread_cond_signal(&pode_escr);
	}


	pthread_mutex_unlock(&mutex);
};

void comeca_escrita(int i)
{
	// a escritora mais rápida será dona da sessão crítica
	pthread_mutex_lock(&mutex);
	nescrita++;
	//se tiver outra escritoras escrevendo ou uma ou mais leitora lendo,  ela espera até receber o sinal
	if (e_cont == 1 || l_cont > 0) {
		++e_esp;
		fprintf(pont_arq, "\ne_entrou_fila(%d,%d)", i,var);
		while(1){
			pthread_cond_wait(&pode_escr, &mutex);
			fprintf(pont_arq, "\ne_recebeu_sinal(%d,%d)", i,var);
			if (e_cont == 0 && l_cont == 0) break;
			fprintf(pont_arq, "\ne_continuou_fila(%d,%d)", i,var);

		}
		--e_esp;
		fprintf(pont_arq, "\ne_saiu_fila(%d,%d)", i,var);
	}
	//Foi liberada, pode escrever
	//Informa que tem uma escritora escrevendo:
	e_cont = 1;
	fprintf(pont_arq, "\nescritora_escreveu(%d,%d)", i,var);
	printf("\nEscritora tá escrevendo a variável %d", i);
	var = i;
	pthread_mutex_unlock(&mutex);
};

void para_escrita(int i)
{
	pthread_mutex_lock(&mutex);
	//Informa que não tem mais thread escreved
	e_cont = 0;
	fprintf(pont_arq, "\ntermina_escrita(%d,%d)", i,var);
	//se tem leitores esperando, elas são liberadas, se não, as escritoras são liberadas
	if (l_esp > 0){
		pthread_cond_broadcast(&podeler);
		fprintf(pont_arq, "\ne_manda_sinal_l(%d,%d)", i,var);
	}
		//pthread_cond_signal(&podeler);
	else{
		pthread_cond_signal(&pode_escr);
		fprintf(pont_arq, "\ne_manda_sinal_e(%d,%d)", i,var);
	}

	pthread_mutex_unlock(&mutex);
};


void* leitor(void* id_le)
{
	int j =0;
	int a = 0;
	int i = *(int*)id_le;
	char nome[dig+4];
	int leit[NLEI];



	printf("\nLeitora %d criada", i);
	fprintf(pont_arq, "\nlei_criada(%d,%d)", i,var);


	//Cada leitora realizará o número NLEI de leituras passado pelo usário, arq
	while (a<NLEI) {
		usleep(1);
		leit[j]=comeca_leitura(i);
		termina_leitura(i);
		a++;
		j++;
	}

	FILE *arq;
	sprintf(nome, "%i", i);
	strcat(nome, ".txt");
	arq = fopen(nome,"wt");

	for(j=0;j<NLEI;j++){
		fprintf(arq, "\n%d",leit[j]);
	}

	fprintf(pont_arq, "\nlei_encerrada(%d,%d)", i,var);
  pthread_exit(NULL);
};

void* escritor(void* id_esc)
{
	int c = 0;
	int i = *(int*)id_esc;

	printf("\nEscritora %d criada", i);
	fprintf(pont_arq, "\nesc_criada(%d,%d)", i,var);

	//Cada escritora realizará o número NESC de leituras passado pelo usário
	while (c<NESCR) {
		usleep(1);
		comeca_escrita(i);
		para_escrita(i);
		c++;
	}

	fprintf(pont_arq, "\nesc_encerrada(%d,%d)", i,var);
  pthread_exit(NULL);

}


int main(int argc, char *argv[])
{
	//Verificando se foram passados a quantidade de argumentos necessárias:
	if(argc < 6) {
    printf("Use: %s <nº de threads leitoras> <nº de threads escritoras> <nº de leituras de cada thread> <nº de escrita de cada thread>  <nome do arquivo de saída>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
	int NTHREADSL, NTHREADSE;
	int num;

	//Número de threads leitoras:
	NTHREADSL = atoi(argv[1]);
	//Número de threads escritoras:
	NTHREADSE = atoi(argv[2]);
	//Número de leituras realizadas por cada thread leitora:
	NLEI = atoi(argv[3]);
	//Número de escritas realizadas por cada thread escritora:
	NESCR = atoi(argv[4]);


	dig = num_Digitos(NTHREADSL);



	//abrindo o arquivo
	pont_arq = fopen(argv[5], "wt");

	if (pont_arq == NULL) // Se não conseguiu criar
	{
	   printf("Problemas na CRIACAO do arquivo\n");
	   return 0;
	}

	fprintf(pont_arq, "\nnum_thread_tipo (%d,%d)", NTHREADSL,NTHREADSE);
	fprintf(pont_arq, "\nnum_exec_thread (%d,%d)", NLEI, NESCR);


	//threads leitoras:
	pthread_t th_le[NTHREADSL];
	int id_le[NTHREADSL];
	//threads escritoras:
	pthread_t th_esc[NTHREADSE];
	int id_esc[NTHREADSE];
	int l = 0, e = 0;

	pthread_cond_init(&podeler, NULL);
	pthread_cond_init(&pode_escr, NULL);
	pthread_mutex_init(&mutex, NULL);


	if (NTHREADSL >= NTHREADSE) num = NTHREADSL;
	else num = NTHREADSE;

	for (int i = 0; i < num; i++) {
		if(i<NTHREADSE){
			id_esc[e] = e;
			e++;
			// criando as threads escritoras
			pthread_create(&th_esc[i], NULL, &escritor, &id_esc[i]);

		}
		if(i<NTHREADSL){
			id_le[l] = l;
			l++;
			// criando as threads leitoras
			pthread_create(&th_le[i], NULL, &leitor, &id_le[i]);

		}
	}

	for (int i = 0; i < NTHREADSL; i++) {
		pthread_join(th_le[i], NULL);;
	}

	for (int i = 0; i < NTHREADSE; i++) {
		pthread_join(th_esc[i], NULL);
	}


}
