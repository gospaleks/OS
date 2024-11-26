#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define LEN 256

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Neodgovarajuci broj argumenata.\n");
        exit(1);
    }

    int pd1[2]; // prema detetu
    int pd2[2]; // prema roditelju

    pipe(pd1);
    pipe(pd2);

    int pid = fork();

    if (pid == 0)
    {
        close(pd1[1]);
        close(pd2[0]);

        char linija[LEN];
        char odgovor[LEN];
        while (read(pd1[0], linija, LEN))
        {
            // karakter je u argv[2]
            if (strstr(linija, argv[2]))
                strcpy(odgovor, "FOUND");
            else
                strcpy(odgovor, "NOT FOUND");

            write(pd2[1], odgovor, sizeof(odgovor));
        }

        close(pd2[1]);
        close(pd1[0]);
    }
    else
    {
        close(pd1[0]);
        close(pd2[1]);

        FILE *f = fopen(argv[1], "r");
        if (f == 0)
            exit(1);

        char linija[LEN];
        while (fgets(linija, LEN, f))
            write(pd1[1], linija, LEN);

        // ovako treba da stoji ovde a nikako dole kao sto bi mahinalno napisali
        close(pd1[1]);

        fclose(f);

        char odgovor[LEN];
        while (read(pd2[0], odgovor, sizeof(odgovor)))
            printf("Poruka: %s\n", odgovor);

        printf("izasao sam iz whileee\n");

        /*
            procesi medjusobno cekaju da se oba kraja pipe-a zatvore
            pa su oba zaglavljena na read u while petlji
            zato gore posle upisa u pd1 zatvaramo i kraj pipe-a za upis da bismo ovo izbegli
        */
        close(pd2[0]);

        wait(NULL);
    }

    return 0;
}