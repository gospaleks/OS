#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#define MAX_LEN 100

/*
	Glavna nit uzima od korisnika broj dok ne unese 'KRAJ'
	dok druga nit odbrojava od 0 do unetog broja sa po 3 sekudne pauze
*/

void * odbrojavaj(void * arg) {
	int broj = *((int *) arg);

	int tmp = 0;
	while (tmp <= broj) {
		printf("Odbrojavam... %d\n", tmp);
		sleep(1); // treba 3
		tmp++;
	}
}

int main(int argc, char * argv[]) {

	int broj;
	char brojStr[MAX_LEN];
	pthread_t nit;

	printf("Unesite ceo broj ili \"KRAJ\" za prekid programa:\n");
	scanf("%s", brojStr);
	while (strcmp(brojStr, "KRAJ") != 0) {

		broj = atoi(brojStr);

		if (broj > 0) {
			pthread_create(&nit, NULL, (void *)odbrojavaj, (void *)&broj);
			pthread_join(nit, NULL);
		}
		
		printf("Unesite ceo broj ili \"KRAJ\" za prekid programa:\n");
		scanf("%s", brojStr);
	}

	return 0;
}