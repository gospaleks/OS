#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define LEN 80

// argv[1] - naziv datoteke, argv[2], argv[3] .... proizvoljan broj kljucnih reci
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Neodgovarajuci broj parametara.\n");
        exit(1);
    }

    int pd1[2];
    int pd2[2];

    pipe(pd1);
    pipe(pd2);

    int pid1, pid2;
    if (pid1 = fork() != 0)
    {
        close(pd1[0]);
        close(pd2[0]);
        close(pd2[1]);

        for (int i = 1; i < argc; ++i)
            write(pd1[1], argv[i], LEN);

        close(pd1[1]);

        wait(NULL);
    }
    else if (pid2 = fork() != 0)
    {
        close(pd2[0]); // pisemo u pd2
        close(pd1[1]); // citamo iz pd1

        char imeFajla[LEN];
        read(pd1[0], imeFajla, LEN);

        // Smesti kljucne reci u niz stringova
        int brKljucnihReci = argc - 2;
        char kljucneReci[brKljucnihReci][LEN];
        int i = 0;
        while (read(pd1[0], kljucneReci[i++], LEN))
            ;

        FILE *f = fopen(imeFajla, "r");
        if (f == 0)
            exit(1);

        // Citaj liniju po liniju i u svaku liniju trazi svaku kljucnu rec sa strstr()
        char linija[200];
        int brTrLinije = 1;
        while (!feof(f))
        {
            fgets(linija, 200, f);

            for (int i = 0; i < brKljucnihReci; ++i)
            {
                if (strstr(linija, kljucneReci[i]) != NULL)
                    write(pd2[1], &brTrLinije, sizeof(int));
            }

            ++brTrLinije;
        }

        fclose(f);

        close(pd2[1]);
        close(pd1[0]);

        wait(NULL);
    }
    else
    {
        close(pd2[1]); // citamo iz pd2
        close(pd1[0]);
        close(pd1[1]);

        int procitaniBroj;
        int suma = 0;
        while (read(pd2[0], &procitaniBroj, sizeof(int)))
        {
            printf("Pronalazak u liniji: %d\n", procitaniBroj);
            suma += procitaniBroj;
        }

        printf("Suma procitanih linija: %d\n", suma);

        close(pd2[0]);
    }

    return 0;
}