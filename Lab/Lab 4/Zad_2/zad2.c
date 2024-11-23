#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	srand(time(NULL));

	// Da pregazi prethodno stanje pre nego pocne da appenduje
	FILE *f = fopen("tmp.txt", "w");
	if (f == 0)
		exit(1);
	fclose(f);

	// Kreiranje deskriptora
	int pd[2];
	if (pipe(pd) < 0)
		exit(1);

	int *niz;
	int n;

	int pid = fork();
	if (pid == 0)
	{
		close(pd[1]); // Zatvori kraj za pisanje

		while (1)
		{
			read(pd[0], &n, sizeof(int));

			if (n == -1)
			{
				close(pd[0]);
				break;
			}

			FILE *f = fopen("tmp.txt", "a");
			if (f == 0)
				exit(1);

			int procitaniBroj;
			for (int i = 0; i < n; ++i)
			{
				read(pd[0], &procitaniBroj, sizeof(int));
				fprintf(f, "%d\n", procitaniBroj);
			}

			fclose(f);
		}
	}
	else
	{
		close(pd[0]); // Zatvori kraj za citanje

		while (1)
		{
			printf("Unesite N: ");
			scanf("%d", &n);

			write(pd[1], &n, sizeof(int));

			if (n == -1)
			{
				close(pd[1]);
				break;
			}

			// Generisanje random brojeva
			niz = (int *)malloc(sizeof(int) * n);
			for (int i = 0; i < n; ++i)
				niz[i] = rand() % 101;

			for (int i = 0; i < n; ++i)
				write(pd[1], &niz[i], sizeof(int));

			free(niz);
		}

		wait(NULL);
	}

	return 0;
}