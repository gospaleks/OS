#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define LEN 80

int main()
{
    int pd1[2];
    int pd2[2];

    pipe(pd1);
    pipe(pd2);

    int pid = fork();
    if (pid == 0)
    {
        close(pd1[1]);
        close(pd2[0]);

        FILE *f = fopen("reci.txt", "r");

        char rec[LEN];
        char trenutnaRecUFajlu[LEN];
        char odgovor[LEN];

        while (read(pd1[0], rec, LEN))
        {
            rewind(f); // vrati kursor na pocetak

            int nadjena = 0;
            while (!nadjena && fscanf(f, "%s", trenutnaRecUFajlu) != EOF)
            {
                if (strcmp(rec, trenutnaRecUFajlu) == 0)
                    nadjena = 1;
            }

            if (nadjena)
                strcpy(odgovor, "FOUND");
            else
                strcpy(odgovor, "NOT FOUND");

            write(pd2[1], odgovor, strlen(odgovor) + 1);
        }

        fclose(f);

        close(pd2[1]);
        close(pd1[0]);
    }
    else
    {
        close(pd1[0]);
        close(pd2[1]);

        char rec[LEN];
        while (1)
        {
            printf("Unesite rec (END za kraj): ");
            scanf("%s", rec);
            if (strcmp(rec, "END") == 0)
                break;

            write(pd1[1], rec, strlen(rec) + 1);

            char odgovor[LEN];
            read(pd2[0], odgovor, LEN);
            printf("Odgovor: %s\n", odgovor);
        }

        close(pd1[1]);
        close(pd2[0]);

        wait(NULL);
    }

    return 0;
}