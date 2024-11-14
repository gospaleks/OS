#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

/*
	Svaka nit sortira posebnu vrstu matrice
	Velicina matrice se unosi kao argument programa
*/

int **matrica;

struct argument_niti {
	int vrsta;
	int M;
};

void * sortiraj(void * arg) {

	struct argument_niti argNiti = *((struct argument_niti*) arg);

	printf("Sortiram %d. vrstu...\n", argNiti.vrsta);

	for (int i = 0; i < argNiti.M - 1; ++i) {
		for (int j = i + 1; j < argNiti.M; ++j) {
			if (matrica[argNiti.vrsta][i] > matrica[argNiti.vrsta][j]) {
				int tmp = matrica[argNiti.vrsta][i];
				matrica[argNiti.vrsta][i] = matrica[argNiti.vrsta][j];
				matrica[argNiti.vrsta][j] = tmp;
			}
		}
	}
}

int main(int argc, char * argv[]) {

	if (argc != 3) {
		printf("Neodgovarajuci broj argumenata.");
		return 1;
	}

	// bez validaciju (neko moz unese kao argument slova)
	// onda atoi vraca 0
	int n = atoi(argv[1]);
	int m = atoi(argv[2]);

	// Zauzmi mem
	matrica = (int**) malloc(sizeof(int*) * n);
	for (int i = 0; i < n; ++i)
		matrica[i] = (int*) malloc(sizeof(int) * m);

	// Neka korisnik unese
	for (int i = 0; i < n; ++i)
		for(int j = 0; j < m; ++j)
			scanf("%d", &matrica[i][j]);

	// Kreiraj nit
	pthread_t niti[n];
	struct argument_niti argumenti[n];
	for (int i = 0; i < n; ++i) {
		argumenti[i].vrsta = i;
		argumenti[i].M = m;
		pthread_create(&niti[i], NULL, (void*)sortiraj, (void*)&argumenti[i]);
	}

	// main ceka svaku nit da se izvrsi
	for (int i = 0; i < n; ++i) {
		pthread_join(niti[i], NULL);
	}

	// stampaj sortiranu matricu
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j)
			printf("%d ", matrica[i][j]);
		printf("\n");
	}

	// Oslobodi mem
	for (int i = 0; i < n; ++i)
		free(matrica[i]);
	free(matrica); 

	return 0;
}