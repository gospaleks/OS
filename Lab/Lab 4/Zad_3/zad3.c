#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define N 20

int main(int argc, char *argv[])
{
    srand(time(NULL));

    int pd[2];
    int niz[N] = {0};

    if (pipe(pd) < 0)
        exit(1);

    int pid = fork();
    if (pid == 0)
    {
        close(pd[1]);

        int broj;
        while (read(pd[0], &broj, sizeof(int)) > 0)
        {
            if (broj % 3 == 0)
                printf("%d\n", broj);
            else
                printf("Broj %d nije deljiv sa 3.\n", broj);
        }

        close(pd[0]);
    }
    else
    {
        close(pd[0]);

        for (int i = 0; i < N; ++i)
        {
            niz[i] = rand() % 100 + 100;
            write(pd[1], &niz[i], sizeof(int));
        }

        close(pd[1]);
        wait(NULL);
    }

    return 0;
}