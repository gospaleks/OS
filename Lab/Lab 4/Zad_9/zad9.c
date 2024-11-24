#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#define N 20

int main()
{
    srand(time(NULL));

    int paranZbir[2];
    if (pipe(paranZbir) < 0)
        return 1;
    int neparanZbir[2];
    if (pipe(neparanZbir) < 0)
        return 1;

    int pidParni = fork();

    if (pidParni == 0)
    {
        /* dete1 - roditelj drugog deteta (proces2) */

        close(paranZbir[1]);

        int niz[N];
        read(paranZbir[0], niz, sizeof(int) * N);

        FILE *f = fopen("BROJEVI.txt", "w");
        for (int i = 0; i < N; i++)
            fprintf(f, "%d\n", niz[i]);
        fclose(f);

        close(paranZbir[0]);

        exit(0);
    }
    else
    {
        int pidNeparni = fork();
        if (pidNeparni == 0)
        {
            /* dete (proces3)*/

            close(neparanZbir[1]);

            int niz[N];
            read(neparanZbir[0], niz, sizeof(int) * N);

            for (int i = 0; i < N; i++)
                printf("%d\n", niz[i]);

            close(neparanZbir[0]);

            exit(0);
        }

        else
        {
            /* roditelj svih (deda) */

            close(paranZbir[0]);
            close(neparanZbir[0]);

            int niz[N];
            int suma = 0;
            for (int i = 0; i < N; i++)
            {
                niz[i] = rand() % 20;
                suma += niz[i];
            }

            if (suma % 2 == 0)
            {
                write(paranZbir[1], niz, sizeof(int) * N);
                kill(pidNeparni, SIGKILL);
            }
            else
            {
                write(neparanZbir[1], niz, sizeof(int) * N);
                kill(pidParni, SIGKILL);
            }

            close(paranZbir[1]);
            close(neparanZbir[1]);

            wait(NULL);
        }
        return 0;
    }
}