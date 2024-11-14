#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

/*
	korisnik unosi ime datoteke i kopira se u tmp folder 
	koji se nalazi u tekucem direktorijumu (mora da postoji)
*/

int main() {

	char imeDatoteke[100];
	scanf("%s", imeDatoteke);

	int pid = fork();

	if (pid == 0) {
		// execlp(imePRograma, argument0, argument1, ....);
		printf("ja sam dete: %d\n", getpid());
		printf("moj roditelj je: %d\n", getppid());
		if (execlp("cp", "cp", imeDatoteke, "./tmp", NULL) < 0) {
			printf("greska\n");
			exit(1);
		}
	}
	else {
		printf("cekam za dete, moj pid je: %d\n", getpid());
		wait(NULL);
	}


	return 0;
}