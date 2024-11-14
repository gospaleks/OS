#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

/*
	Nit koja ispisuje aritm sredinu niza
	elementi niza se unose kao argumenti programa
	parametri funkcije koju izvrsava nit su niz i broj elemenata
*/

typedef struct arg_niti {
	int *niz;
	int n;
} arg_niti ;

void * aritmSredina(void * arg) {
	arg_niti args = *((arg_niti *) arg);

	double suma = 0.0;
	for (int i = 0; i < args.n; ++i) {
		suma += args.niz[i];
	}

	printf("Aritmeticka sredina niza je: %.3lf\n", suma / args.n);
}

int main(int argc, char* argv[]) {

	int *niz = (int*) malloc(sizeof(int) * (argc - 1));

	for (int i = 1; i < argc; ++i) {
		niz[i-1] = atoi(argv[i]);
	}

	pthread_t nit;
	
	arg_niti arg;
	arg.niz = niz;
	arg.n = argc - 1;

	pthread_create(&nit, NULL, (void *)aritmSredina, (void *)&arg);
	pthread_join(nit, NULL);

	free(niz);

	return 0;
}