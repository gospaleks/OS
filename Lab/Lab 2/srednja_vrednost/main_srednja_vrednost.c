#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
/*
	Sa stdin cita N celih brojeva i prosledjuje ./srednja_vrednost da printa sr. vrednost
*/

int main(int argc, char * argv[]) {

	int n;
	char broj[100];

	char **arguments = (char**) malloc(sizeof(char*) * (n + 2));
	arguments[0] = (char*) malloc(sizeof(char) * (strlen("./srednja_vrednost") + 1));
	strcpy(arguments[0], "./srednja_vrednost");

	printf("Unesite N: ");
	scanf("%d", &n);

	printf("Unesite %d celih brojeva:\n", n);
	for (int i = 1; i <= n; ++i) {
		scanf("%s", broj);
		arguments[i] = (char*) malloc(sizeof(char) * (strlen(broj) + 1));
		strcpy(arguments[i], broj);
	}

	arguments[n+1] = NULL;
	int pid = fork(); // nemoj u if fork() jer jebe nesto s oslobadjanje memoriju
	if (pid == 0) {
		if (execv("./srednja_vrednost", arguments) < 0) {
			printf("Doslo je do greske prilikom poziva ./srednja_vrednost !\n");
			exit(1);
		}
	}
	else {
		wait(NULL);
	}

	// Oslobodi memoriju
	for (int i = 0; i <= n; ++i)
		free(arguments[i]);
	free(arguments);

	return 0;
}