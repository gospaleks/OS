#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main()
{
    int pd[2];
    pipe(pd);

    int pid = fork();
    if (pid != 0)
    {
        // roditelj
        close(pd[0]);

        srand(time(NULL));
        int neparni_uzastopni = 0;

        while (neparni_uzastopni < 3)
        {
            int broj = rand() % 100;
            write(pd[1], &broj, sizeof(int));

            if (broj % 2 != 0)
                neparni_uzastopni++;
            else
                neparni_uzastopni = 0;
        }

        close(pd[1]);
        wait(NULL);
    }
    else
    {
        // dete
        close(pd[1]); // zatvori kraj za pisanje

        int broj;
        while (read(pd[0], &broj, sizeof(int)) > 0)
        {
            if (broj % 2 == 0)
                printf("Paran broj: %d\n", broj);
        }

        close(pd[0]);
    }

    return 0;
}