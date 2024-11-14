#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct thread_data {
	int* arr;
	int n;
	int brojKolone;
};

void * findMaxElement(void * arg) {
	struct thread_data * td = (struct thread_data *) arg;

	int maxEl = td->arr[0];
	for (int i = 1; i < td->n; ++i)
		if (td->arr[i] > maxEl)
			maxEl = td->arr[i];

	printf("Maksimalni element %d. kolone: %d\n", td->brojKolone, maxEl);

	// oslobadjanje memorije za onaj tmpArr
	free(td->arr);
}

int main(int argc, char * argv[]) {

	if (argc != 3) {
		printf("Neodgovarajuci broj parametara.\n");
		return 1;
	}

	int N = atoi(argv[1]);
	int M = atoi(argv[2]);

	// Zauzmi mem za matricu
	int **matrica = malloc(N * sizeof(int*));
	for (int i = 0; i < N; ++i)
		matrica[i] = malloc(M * sizeof(int));

	printf("Matrica %d x %d\n", N, M);
	printf("Unesite elemente matrice:\n");

	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
			scanf("%d", &matrica[i][j]);

	pthread_t niti[M]; 				// Za svaku kolonu jedna nit
	struct thread_data args[M]; 	// Argumenti za svaku nit

	for (int i = 0; i < M; ++i) {

		// Popuni tmp niz sa elementima kolone
		int *tmpArr = malloc(N * sizeof(int));
		for (int j = 0; j < N; ++j)
			tmpArr[j] = matrica[j][i];

		args[i].arr = tmpArr;
		args[i].n = N;
		args[i].brojKolone = i;

		pthread_create(&niti[i], NULL, (void *)findMaxElement, (void *)&args[i]);
	}

	// Main thread ceka da se ostale niti zavrse
	for (int i = 0; i < N; ++i)
		pthread_join(niti[i], NULL);

	// Oslobodi mem za matricu
	for (int i = 0; i < N; ++i)
		free(matrica[i]);
	free(matrica);


	return 0;
}