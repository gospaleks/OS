#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define N 5

int main(int argc, char **argv)
{
    srand(time(NULL));

    int pd1[2];
    int pd2[2];

    pipe(pd1);
    pipe(pd2);

    int pid = fork();
    if (pid == 0)
    {
        close(pd1[1]);
        close(pd2[0]);

        int procitaniBroj;
        while (read(pd1[0], &procitaniBroj, sizeof(procitaniBroj)))
        {
            if (procitaniBroj % 2 == 0)
            {
                printf("Vracam: %d\n", procitaniBroj);
                write(pd2[1], &procitaniBroj, sizeof(procitaniBroj));
            }
        }

        close(pd2[1]);
        close(pd1[0]);
    }
    else
    {
        close(pd1[0]);
        close(pd2[1]);

        int generisaniBroj;
        for (int i = 0; i < N; ++i)
        {
            generisaniBroj = rand() % 100;
            printf("Generisao sam i saljem: %d\n", generisaniBroj);
            write(pd1[1], &generisaniBroj, sizeof(generisaniBroj));
        }
        close(pd1[1]); // zatvori pipe da proces dete moze da izadje iz while-a

        int vraceniBroj;
        int brParnih = 0;
        while (read(pd2[0], &vraceniBroj, sizeof(vraceniBroj)))
            ++brParnih;

        printf("Broj parnih: %d\n", brParnih);

        close(pd2[0]);
    }

    return 0;
}