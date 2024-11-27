#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <time.h>

#define N 10

int main()
{
    srand(time(NULL));

    int generisanje[2];
    int stampanje[2];

    pipe(generisanje);
    pipe(stampanje);

    int pid1, pid2;

    if (pid1 = fork() != 0)
    {
        /* roditelj - generise brojeve*/

        close(stampanje[0]);
        close(stampanje[1]);
        close(generisanje[0]);

        for (int i = 0; i < N; i++)
        {
            int broj = rand() % 100;
            write(generisanje[1], &broj, sizeof(int));
        }

        close(generisanje[1]);

        wait(NULL);
        wait(NULL);
    }
    else if (pid2 = fork() != 0)
    {
        /* 2. proces - obradjuje primljene brojeve */

        close(generisanje[1]);
        close(stampanje[0]);

        int broj;
        while (read(generisanje[0], &broj, sizeof(int)) > 0)
        {
            if (broj % 2 == 0)
                broj = broj / 2;

            write(stampanje[1], &broj, sizeof(int));
        }

        close(generisanje[0]);
        close(stampanje[1]);
        exit(0);
    }
    else
    {
        /* 3. proces - stampa */

        close(generisanje[0]);
        close(generisanje[1]);
        close(stampanje[1]);

        int fd = open("redirekcija.txt", O_CREAT | O_TRUNC | O_RDWR, 0666);
        dup2(fd, 1);

        int broj;
        while (read(stampanje[0], &broj, sizeof(int)) > 0)
            if (broj > 10)
                printf("%d\n", broj);

        close(fd);

        close(stampanje[0]);
        exit(0);
    }
    return 0;
}