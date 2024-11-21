#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define MAX_LEN 100

sem_t citanje, suma;

int *niz = NULL;
int n;
int izracunataSuma;
int kraj = 0;

void *sumiraj(void *arg)
{
    while (1)
    {
        sem_wait(&suma);

        if (kraj)
            break;

        izracunataSuma = 0;
        for (int i = 0; i < n; i++)
            izracunataSuma += niz[i];

        sem_post(&citanje);
    }
    return NULL;
}

int main()
{
    pthread_t t;

    sem_init(&citanje, 0, 1);
    sem_init(&suma, 0, 0);

    pthread_create(&t, NULL, (void *)sumiraj, NULL);

    FILE *f = fopen("brojevi.txt", "r");

    if (f == 0)
    {
        printf("Neuspesno otvaranje fajla");
        exit(1);
    }

    char linija[MAX_LEN];
    while (!feof(f))
    {
        fgets(linija, MAX_LEN, f);

        sem_wait(&citanje);

        // Preskakanje prve iteracije
        if (niz != NULL)
        {
            printf("Suma: %d\n", izracunataSuma);
            free(niz);
        }

        // Racunanje broja elemenata
        n = 0;
        int i = 0;
        while (linija[i] != '\0')
        {
            if (linija[i] == ' ')
                ++n;
            ++i;
        }
        ++n;

        // Alokacija niza i ubacivanje elemenata
        niz = (int *)malloc(sizeof(int) * n);

        char *str = strtok(linija, " ");

        i = 0;
        while (str != NULL)
        {
            niz[i++] = atoi(str);
            str = strtok(NULL, " ");
        }

        sem_post(&suma);
    }

    kraj = 1;

    fclose(f);

    pthread_join(t, NULL);

    sem_destroy(&citanje);
    sem_destroy(&suma);

    return 0;
}