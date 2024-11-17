#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>

#define N 4

// deljivi resurs
int mat[N][N];

sem_t s1, s2;

void stampajMatricu() {
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j)
			printf("%d ", mat[i][j]);
		printf("\n");
	}
}

void * sortVrsta(void * arg) {

	for (int i = 0; i < N; ++i) { // za svaku vrstu

		sem_wait(&s1);
		
		// Sort
		for (int j = 0; j < N - 1; ++j)
			for (int k = j + 1; k < N; ++k)
				if (mat[i][j] > mat[i][k]) {
					int tmp = mat[i][j];
					mat[i][j] = mat[i][k];
					mat[i][k] = tmp; 
				}

		printf("nit_vrste %d:\n", i);
		stampajMatricu();
		printf("\n");
		
		sem_post(&s2);

		sleep(2);
	}

}

void * sortKolona(void * arg) {
	
	for (int i = 0; i < N; ++i) { // za svaku kolonu

		sem_wait(&s2);

		// Sort
		for (int j = 0; j < N - 1; ++j)
			for (int k = j + 1; k < N; ++k)
				if (mat[j][i] > mat[k][i]) {
					int tmp = mat[j][i];
					mat[j][i] = mat[k][i];
					mat[k][i] = tmp;
				}

		printf("nit_kolone %d:\n", i);
		stampajMatricu();
		printf("\n");

		sem_post(&s1);

		sleep(2);
	}
}

int main() {

	// Unos matrice
	printf("Unesite matricu velicine %dx%d:\n", N, N);
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			scanf("%d", &mat[i][j]);
	
	// pravimo klackalicu
	sem_init(&s1, 0, 1);
	sem_init(&s2, 0, 0);

	pthread_t t1, t2;

	pthread_create(&t1, NULL, (void *) sortVrsta, NULL);
	pthread_create(&t2, NULL, (void *) sortKolona, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	sem_destroy(&s1);
	sem_destroy(&s2);

	stampajMatricu();

	return 0;
}