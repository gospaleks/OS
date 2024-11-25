#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define LEN 1000

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Neodgovarajuci broj argumenata.\n");
        exit(1);
    }

    int pd[2];
    pipe(pd);

    int pid = fork();
    if (pid == 0)
    {
        close(pd[1]);

        FILE *f = fopen(argv[2], "w");
        if (f == 0)
            exit(1);

        char procitaniBajt;
        int brBajta = 1;
        while (read(pd[0], &procitaniBajt, sizeof(char)))
        {
            int xorVr = procitaniBajt ^ brBajta;
            fprintf(f, "%d%c", xorVr, procitaniBajt);
            ++brBajta;
        }

        fclose(f);

        close(pd[0]);
    }
    else
    {
        close(pd[0]);

        FILE *f = fopen(argv[1], "r");
        if (f == 0)
            exit(1);

        char buff[LEN];
        while (fread(buff, LEN, 1, f))
        {
            for (int i = 0; i < LEN; ++i)
                write(pd[1], &buff[i], sizeof(char));
        }

        fclose(f);

        close(pd[1]);

        wait(NULL);
    }

    return 0;
}