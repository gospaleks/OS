#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
	Program koji kompajlira fajl, prosledjen kao argument programu
	koriscenjem gcc kompajlera tako da se kompajlirani program naziva
	'program' i da ima podrsku za debagiranje
*/

int main(int argc, char * argv[]) {

	if (argc != 2) {
		printf("Neodgovarajuci broj argumenata.\n");
		return 1;
	}

	char * imeFajla = malloc((strlen(argv[1]) + 1) * sizeof(char));
	strcpy(imeFajla, argv[1]);

	if (fork() == 0) {
		if (execlp("gcc", "gcc", imeFajla, "-o", "program", "-g", NULL) < 0) {
			printf("Doslo je do greske!\n");
			exit(1);
		}
	}
	else {
		wait(NULL);
	}

	free(imeFajla);

	return 0;
}