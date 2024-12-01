#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define BR_ITER 5 // treba 4096
#define N 5	// treba 1024

void prikaziNaStdout(int sigNum)
{
	FILE * f = fopen("prenos.dat", "r");
	int broj;
	while (fscanf(f, "%d", &broj) != EOF)
		printf("%d\n", broj);

	printf("\n");
	fclose(f);
}

void pisiUFajl(int sigNum)
{
	FILE * f = fopen("prenos.dat", "w");
	
	int i = 0;
	while (i < N)
	{
		fprintf(f, "%d\n", rand() % 101);
		++i;
	}

	fclose(f);
}

int main(int argc, char ** argv)
{
	srand(time(NULL));

	signal(SIGUSR1, prikaziNaStdout);
	signal(SIGUSR2, pisiUFajl);

	int pid = fork();
	if (pid != 0)
	{
		// roditelj
		int brIt = 0;
		while (brIt < BR_ITER)
		{
			// ceka da krene (dete ga aktivira)
			pause();

			// aktivira dete i ceka sledecu iteraciju
			kill(pid, SIGUSR1);
			++brIt;
		}
	}
	else
	{
		do
		{
			// aktiviraj roditeljevu proceduru
			kill(getppid(), SIGUSR2);
		
			// dete ceka SIGUSR1
			pause();
		} while (1);
	}

	return 0;
}