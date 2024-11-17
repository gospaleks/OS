#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>

#define N 20

sem_t par, nepar;

void * parF(void * arg) {

	FILE* f = (FILE *) arg;

	for (int i = 2; i <= N; i += 2) {
		sem_wait(&par);
		fprintf(f, "%d\n", i);
		sem_post(&nepar);
	}
}

void * neparF(void * arg) {
	FILE* f = (FILE *) arg;

	for (int i = 1; i < N; i += 2) {
		sem_wait(&nepar);
		fprintf(f, "%d\n", i);
		sem_post(&par);
	}
}

int main() {

	char imeFajla[100];
	printf("Unesite ime fajla: ");
	scanf("%s", imeFajla);

	FILE * f = fopen(imeFajla, "w");

	if (f == NULL) {
		printf("Greska pri otvaranju fajla.\n");
		return 1;
	}

	sem_init(&par, 0, 0); 
	sem_init(&nepar, 0, 1);

	pthread_t t1, t2;

	pthread_create(&t1, NULL, (void*)parF, (void*)f);
	pthread_create(&t2, NULL, (void*)neparF, (void*)f);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	sem_destroy(&par);
	sem_destroy(&nepar);

	fclose(f);

	return 0;
}