#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

#define N 15

int main(int argc, char **argv)
{
	srand(time(NULL));

	// Kreiraj datavod
	int pd[2];
	if (pipe(pd) < 0)
		exit(1);

	// Popuni random brojevima
	int niz[N];
	for (int i = 0; i < N; ++i)
		niz[i] = rand() % 101;

	// Ako je prvi paran
	if (niz[0] % 2 == 0)
	{
		int pid = fork();
		if (pid == 0)
		{
			close(pd[1]);

			FILE *f = fopen("PARNI.txt", "w");
			int procitaniBroj;

			for (int i = 0; i < N; ++i)
			{
				read(pd[0], &procitaniBroj, sizeof(int));
				fprintf(f, "%d\n", procitaniBroj);
			}

			fclose(f);

			close(pd[0]);
		}
		else
		{
			close(pd[0]);

			for (int i = 0; i < N; ++i)
				write(pd[1], &niz[i], sizeof(int));

			close(pd[1]);

			wait(NULL);
		}
	}
	// Ako je neparan
	else
	{
		int pid = fork();
		if (pid == 0)
		{
			close(pd[1]);

			int procitaniBroj;
			for (int i = 0; i < N; ++i)
			{
				read(pd[0], &procitaniBroj, sizeof(int));
				printf("%d\n", procitaniBroj);
			}

			close(pd[0]);
		}
		else
		{
			close(pd[0]);

			for (int i = 0; i < N; ++i)
				write(pd[1], &niz[i], sizeof(int));

			close(pd[1]);
			wait(NULL);
		}
	}

	return 0;
}