#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

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
        FILE *f = fopen("tmp.txt", "w");

        while (1)
        {
            read(pd[0], &broj, sizeof(int));

            if (broj == -1)
            {
                close(pd[0]);
                fclose(f);
                break;
            }
            fprintf(f, "%d\n", broj);
        }
    }
    else
    {
        close(pd[0]);

        int n;

        while (1)
        {
            printf("uneti N:\n");
            scanf("%d", &n);

            if (n == -1)
            {
                write(pd[1], &n, sizeof(int));
                close(pd[1]);
                break;
            }
            int rndbroj;
            for (int i = 0; i < n; i++)
            {
                rndbroj = rand() % 1001;
                write(pd[1], &rndbroj, sizeof(int));
            }
        }
        wait(NULL);
    }
    return 0;
}