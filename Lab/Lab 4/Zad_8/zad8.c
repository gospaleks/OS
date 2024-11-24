#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define N 20

int main()
{
    srand(time(NULL));

    int pd[2];
    if (pipe(pd) < 0)
        return 1;

    int pid = fork();

    if (pid == 0)
    {
        close(pd[1]);
        int broj;
        while (read(pd[0], &broj, sizeof(int)) > 0)
        {
            if (broj % 2 == 0)
                printf("%d\n", broj);
            else
                printf("-1\n");
        }
        close(pd[0]);
    }
    else
    {
        close(pd[0]);
        int broj;
        for (int i = 0; i < N; i++)
        {
            broj = rand() % 191 + 10;
            write(pd[1], &broj, sizeof(int));
        }
        close(pd[1]);
        wait(NULL);
    }
    return 0;
}