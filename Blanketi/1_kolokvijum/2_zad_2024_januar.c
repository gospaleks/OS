#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define LEN 256

int main()
{
    int pdReci[2], pdOdgovor[2];

    pipe(pdReci);
    pipe(pdOdgovor);

    int pid = fork();

    if (pid != 0)
    {
        /* roditelj */

        close(pdReci[0]);
        close(pdOdgovor[1]);

        char recenica[LEN];

        printf("uneti recenicu:\n");
        fgets(recenica, LEN, stdin);

        int n = 0;
        int i = 0;
        while (recenica[i] != '\0')
        {
            if (recenica[i] == ' ')
                n++;
            i++;
        }
        n++;

        char **reci;
        reci = (char **)malloc(sizeof(char *) * n);

        char *tmpRec = strtok(recenica, " ");

        i = 0;
        while (tmpRec != NULL)
        {
            reci[i] = (char *)malloc(sizeof(char) * (strlen(tmpRec) + 1));
            strcpy(reci[i], tmpRec);
            tmpRec = strtok(NULL, " ");
            i++;
        }

        int sumaCifara;
        for (int i = 0; i < n; i++)
        {
            write(pdReci[1], reci[i], LEN);
            read(pdOdgovor[0], &sumaCifara, sizeof(int));
            printf("zbir cifara u reci %s je %d\n", reci[i], sumaCifara);
        }

        close(pdReci[1]);
        close(pdOdgovor[0]);

        for (int i = 0; i < n; i++)
            free(reci[i]);
        free(reci);

        wait(NULL);
    }
    else
    {
        /* dete */

        close(pdReci[1]);
        close(pdOdgovor[0]);

        char rec[LEN];
        while (read(pdReci[0], rec, LEN))
        {
            int suma = 0;

            for (int i = 0; i < strlen(rec); i++)
                if (rec[i] >= '0' && rec[i] <= '9')
                    suma += rec[i] - '0';

            write(pdOdgovor[1], &suma, sizeof(int));
        }

        close(pdReci[0]);
        close(pdOdgovor[1]);
    }
    return 0;
}