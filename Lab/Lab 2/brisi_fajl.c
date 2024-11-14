#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

/*
	Brise fajl koji je programu prosledjen kao argument programa
	pomocu komande rm
*/

int main(int argc, char * argv[]) {

	if (argc != 2) {
		printf("Neodgovarajuci broj argumenata.");
		return 1;
	}

	// u argv[1] je ime datoteke koju brisemo

	int pid = fork();
	if (pid == 0) {

		if (execlp("rm", "rm", argv[1], NULL) < 0) {
			printf("Doslo je do greske.\n");
			exit(1);
		}

	}
	else {
		wait(NULL);
	}

	return 0;
}