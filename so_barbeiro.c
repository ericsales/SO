//Bibliotecas
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//Defines
#define N_CLIENTES 20
#define N_CADEIRAS 4
#define N_BARBEIROS 3

//semaforos
sem_t semaforo_salao;
sem_t semaforo_sofa;
sem_t semaforo_cadeira_barbeiro;
sem_t semaforo_cabelo_cortado;
sem_t semaforo_cliente_cadeira;

//Funcao barbeiro
void *f_barbeiro(void *v){

	while(1) {
		sem_wait(&semaforo_cliente_cadeira);
		printf("Barbeiro cortou o cabelo de um cliente.\n");
		sem_post(&semaforo_cabelo_cortado);
	}
	return NULL;
}


//funcao cliente
void *f_cliente(void* v){
	int id = *(int*) v;

	sleep(id%3);
	if(sem_trywait(&semaforo_salao) == 0){
		printf("Cliente %d entrou na barbearia.\n", id);

		//esperando ligar no SOFA
		sem_wait(&semaforo_sofa);
		printf("Cliente %d sentou no sofa.\n", id);

		//ESPERANDO CORTAR CABELO
		sem_wait(&semaforo_cadeira_barbeiro);
		sem_post(&semaforo_cliente_cadeira);
		printf("Cliente %d sentou na cadeira de barbeiro.\n", id);
		sem_post(&semaforo_sofa);

		sem_wait(&semaforo_cabelo_cortado);
		sem_post(&semaforo_cadeira_barbeiro);

		sem_post(&semaforo_salao);
		printf("Cliente %d deixou a barbearia.\n", id);
	}else{
		printf("Cliente %d nao entrou na barbearia.\n", id);
	}

	return NULL;
}

int main(){
	//Inicializaçoes
	int id[N_CLIENTES];
	pthread_t th_clientes[N_CLIENTES], th_barbeiro[N_BARBEIROS];

	//Iniciando semaforos
	sem_init(&semaforo_salao, 0, N_CLIENTES);
	sem_init(&semaforo_sofa, 0, N_CADEIRAS);
	sem_init(&semaforo_cadeira_barbeiro, 0, N_BARBEIROS);
	sem_init(&semaforo_cabelo_cortado, 0, 0);
	sem_init(&semaforo_cliente_cadeira, 0, 0);

	//Criando threads clientes
	for (int i = 0; i < N_CLIENTES; i++) {
		id[i] = i;
		pthread_create(&th_clientes[i], NULL, f_cliente, (void*) &id[i]);
	}

	//Criando threads barbeiros
	for (int i = 0; i < N_BARBEIROS; i++) {
		id[i] = i;
		pthread_create(&th_barbeiro[i], NULL, f_barbeiro, (void*) &id[i]);
	}

	//Iniciando threads clientes
	for (int i = 0; i < N_CLIENTES; i++) 
		pthread_join(th_clientes[i], NULL);
	
	return 0;
}