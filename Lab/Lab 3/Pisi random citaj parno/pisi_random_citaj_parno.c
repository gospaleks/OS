#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 5

// deljivi resurs
int niz[N] = { 0 };

pthread_mutex_t lock;
pthread_cond_t pisanje;
int mozePisanje = 1;
pthread_cond_t citanje;
int mozeCitanje = 0;

void * nit1(void * arg) {

	while (1) {
		pthread_mutex_lock(&lock);
		
		while (!mozePisanje)
			pthread_cond_wait(&pisanje, &lock);

		int randomPoz = rand() % N;
		int randomVr = rand() % 20 - 9;
		niz[randomPoz] = randomVr;

		printf("na poz %d upisano %d\n", randomPoz, randomVr);

		mozePisanje = 0;
		mozeCitanje = 1;
		pthread_cond_signal(&citanje);
		
		pthread_mutex_unlock(&lock);

		// sleep ispod unlock da ne drzi 3 sekunde zakljucan mutex
		sleep(3);
	}

}

void * nit2(void * arg) {
	
	while (1) {
		pthread_mutex_lock(&lock);
		
		while (!mozeCitanje)
			pthread_cond_wait(&citanje, &lock);

		int suma = 0;
		for (int i = 0; i < N; ++i) 
			suma += niz[i];

		if (!(suma & 1)) {
			printf("Niz nakon upisa prve niti:\n");
			for (int i = 0; i < N; ++i)
				printf("%d ", niz[i]);
			printf("\n");
		}
		else {
			printf("neparno je\n");
		}

		mozeCitanje = 0;
		mozePisanje = 1;
		pthread_cond_signal(&pisanje);
		
		pthread_mutex_unlock(&lock);
	}

}

int main(int argc, char * argv[]) {

	srand(time(NULL));

	pthread_t t1, t2;

	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&pisanje, NULL);
	pthread_cond_init(&citanje, NULL);

	pthread_create(&t1, NULL, (void*)nit1, NULL);
	pthread_create(&t2, NULL, (void*)nit2, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("\n");

	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&pisanje);
	pthread_cond_destroy(&citanje);

	return 0;
}