#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_LEN 256

/*
	2023 Kol I
	Grupa II-b
*/

char recenica[MAX_LEN];

sem_t s, s1, s2;

// Ova promenljiva je potrebna kako bi se pravilno sve unistilo
int kraj = 0;

void * nit1(void * arg) {

	while (1) {
		sem_wait(&s1);

		if (kraj) {
			sem_post(&s2);
			break;
		}

		recenica[0] = toupper(recenica[0]);
		int n = strlen(recenica);

		// Kada se unese sa fgets() u stringu recenica ostane i '\n' kao posledji karakter pa to mora da se pazi
		if (recenica[n - 2] != '.') {
			recenica[n-1] = '.';
			recenica[n] = '\n';
			recenica[n+1] = '\0';
		}

		sem_post(&s2);
	}

}

void * nit2(void * arg) {

	while (1) {
		sem_wait(&s2);

		if (kraj) {
			sem_post(&s);
			break;
		}

		int i = 0;
		while (recenica[i] != '\0') {
			if (recenica[i] == ' ')
				recenica[i] = '_';
			++i;
		}

		printf("Izmenjena recenica: %s", recenica);

		sem_post(&s);
	}
}

int main() {

	sem_init(&s, 0, 1);
	sem_init(&s1, 0, 0);
	sem_init(&s2, 0, 0);

	pthread_t t1, t2;

	pthread_create(&t1, NULL, (void*)nit1, NULL);
	pthread_create(&t2, NULL, (void*)nit2, NULL);

	while (!kraj) {
		sem_wait(&s);

		printf("Unesite recenicu:\n");
		fgets(recenica, MAX_LEN, stdin);

		if (strcmp(recenica, "KRAJ\n") == 0) {
			kraj = 1;
			sem_post(&s1); // omoguci nit 1 da zavrsi
			sem_post(&s2); // omoguci nit 2 da zavrsi
		}
		
		sem_post(&s1);

	}

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	sem_destroy(&s);
	sem_destroy(&s1);
	sem_destroy(&s2);

	return 0;
}