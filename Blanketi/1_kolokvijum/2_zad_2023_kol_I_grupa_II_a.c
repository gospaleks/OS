#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define LEN 256

int main()
{
    int pdPremaDetetu[2];
    int pdPremaRoditelju[2];

    pipe(pdPremaDetetu);
    pipe(pdPremaRoditelju);

    int pid = fork();
    if (pid == 0)
    {
        close(pdPremaRoditelju[0]);
        close(pdPremaDetetu[1]);

        char imeFajla[LEN];
        char kljucnaRec[LEN];

        read(pdPremaDetetu[0], imeFajla, LEN);
        read(pdPremaDetetu[0], kljucnaRec, LEN);

        FILE *f = fopen(imeFajla, "r");
        if (f == 0)
            exit(1);

        char linija[LEN];
        int brLinije = 1;
        while (fgets(linija, LEN, f))
        {
            if (strstr(linija, kljucnaRec))
                write(pdPremaRoditelju[1], &brLinije, sizeof(int));

            brLinije++;
        }

        fclose(f);

        close(pdPremaRoditelju[1]);
        close(pdPremaDetetu[0]);
    }
    else
    {
        close(pdPremaDetetu[0]);
        close(pdPremaRoditelju[1]);

        char imeFajla[LEN];
        char kljucnaRec[LEN];

        printf("uneti putanju fajla:\n");
        scanf("%s", imeFajla);
        printf("uneti kljucnu rec:\n");
        scanf("%s", kljucnaRec);

        write(pdPremaDetetu[1], imeFajla, LEN);
        write(pdPremaDetetu[1], kljucnaRec, LEN);

        int brLinije;
        while (read(pdPremaRoditelju[0], &brLinije, sizeof(int)))
            printf("Kljucna rec %s nadjena u liniji %d\n", kljucnaRec, brLinije);

        close(pdPremaRoditelju[0]);
        close(pdPremaDetetu[1]);

        wait(NULL);
    }
    return 0;
}