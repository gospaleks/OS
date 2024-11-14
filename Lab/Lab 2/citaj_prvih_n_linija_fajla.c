#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
	Program prihvata dva argumenta: ime fajla i broj linija 
	Treba procitati zadaci broj linija od pocetka fajla koristeci odgovarajuci execl funkciju
*/

int main(int argc, char * argv[]) {

	if (argc != 3) {
		printf("Neodgovarajuci broj parametara.\n");
		return 1;
	}

	char* imeFajlaArg = malloc((strlen(argv[1]) + 1) * sizeof(char));
	strcpy(imeFajlaArg, argv[1]);

	// +2 jer treba dodamo '-' ispred broja linija
	char* brojLinijaArg = malloc((strlen(argv[2]) + 2) * sizeof(char));
	strcpy(brojLinijaArg, "-");
	strcat(brojLinijaArg, argv[2]);

	// printf("Broj linija arg: %s\n", brojLinijaArg);
	// printf("Ime fajla arg  : %s\n", imeFajlaArg);

	if (fork() == 0) {
		if (execlp("head", "head", brojLinijaArg, imeFajlaArg, NULL) < 0) {
			printf("Doslo je do greske!\n");
			exit(1);
		}
	}
	else {
		// roditeljski proces ceka da se dete zavrsi
		wait(NULL);
	}

	free(imeFajlaArg);
	free(brojLinijaArg);

	return 0;
}