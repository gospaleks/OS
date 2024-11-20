#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 10

int niz[N];
int zbirNiza;

sem_t zbir, randomUpis;

void *randomUpisBrojeva(void *arg)
{
    while (1)
    {
        sem_wait(&randomUpis);

        if (zbirNiza > 1000)
            break;

        for (int i = 0; i < N; i++)
            niz[i] = rand() % 200;

        sem_post(&zbir);
    }
    return NULL;
}

int main()
{
    srand(time(NULL));

    sem_init(&zbir, 0, 0);
    sem_init(&randomUpis, 0, 1);

    pthread_t drugaNit;

    pthread_create(&drugaNit, NULL, (void *)randomUpisBrojeva, NULL);

    do
    {
        sem_wait(&zbir);

        zbirNiza = 0;
        for (int i = 0; i < N; i++)
            zbirNiza += niz[i];

        if (zbirNiza > 800)
            printf("zbri niza je VECI od 800\n");
        else
            printf("zbir niza je MANJI od 800\n");

        sem_post(&randomUpis);

    } while (zbirNiza <= 1000);

    printf("\nzbir niza je VECI od 1000, KRAJ\n");

    pthread_join(drugaNit, NULL);

    sem_destroy(&zbir);
    sem_destroy(&randomUpis);

    return 0;
}