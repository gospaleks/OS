#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define MAX_LEN 100

char linija[MAX_LEN];
int kraj = 0;

sem_t neparne, parne, glavni;

void *parneLinije(void *arg)
{
    FILE *f = fopen("parni.txt", "w");
    if (f == 0)
        return NULL;

    while (1)
    {
        sem_wait(&parne);

        if (kraj)
            break;

        fprintf(f, "%s\n", linija);

        sem_post(&glavni);
    }
    return NULL;
}

void *neparneLinije(void *arg)
{
    FILE *f = fopen("neparni.txt", "w");
    if (f == 0)
        return NULL;

    while (1)
    {
        sem_wait(&neparne);

        if (kraj)
            break;

        fprintf(f, "%s\n", linija);

        sem_post(&glavni);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Neodgovarajuci broj argumenata.\n");
        exit(1);
    }

    FILE *f = fopen(argv[1], "r");
    if (f == 0)
    {
        printf("Greska prilikom citanja.\n");
        exit(1);
    }

    pthread_t neparneNit, parneNit;
    pthread_create(&neparneNit, NULL, (void *)neparneLinije, NULL);
    pthread_create(&parneNit, NULL, (void *)parneLinije, NULL);

    sem_init(&neparne, 0, 0);
    sem_init(&parne, 0, 0);
    sem_init(&glavni, 0, 1);

    int i = 1;
    while (!feof(f))
    {
        sem_wait(&glavni);

        fgets(linija, MAX_LEN, f);

        printf("%s\n", linija);

        if (i)
        {
            sem_post(&neparne);
            i = 0;
        }
        else
        {
            sem_post(&parne);
            i = 1;
        }
    }

    sem_wait(&glavni);
    kraj = 1;

    sem_post(&neparne);
    sem_post(&parne);

    fclose(f);

    pthread_join(neparneNit, NULL);
    pthread_join(parneNit, NULL);

    sem_destroy(&neparne);
    sem_destroy(&parne);
    sem_destroy(&glavni);

    return 0;
}