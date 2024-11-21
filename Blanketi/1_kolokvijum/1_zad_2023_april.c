#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

/* uvek je veci od 2000 koliko sam ja testirao */

#define N 10
#define M 10

int niz[N];
int zbirNiza;

sem_t upis, provera;

void *parnaMesta(void *arg)
{
    for (int i = 0; i < M; i++)
    {
        sem_wait(&upis);

        for (int j = 0; j < N; j += 2)
            niz[j] = rand() % 300;

        sem_post(&provera);
    }
    return NULL;
}

void *neparnaMesta(void *arg)
{
    for (int i = 0; i < M; i++)
    {
        sem_wait(&upis);

        for (int j = 1; j < N; j += 2)
            niz[j] = (rand() % 200) + 300;

        sem_post(&provera);
    }
    return NULL;
}

int main()
{
    srand(time(NULL));

    sem_init(&upis, 0, 2);
    sem_init(&provera, 0, 0);

    pthread_t t1, t2;

    pthread_create(&t1, NULL, (void *)parnaMesta, NULL);
    pthread_create(&t2, NULL, (void *)neparnaMesta, NULL);

    for (int i = 0; i < M; i++)
    {
        sem_wait(&provera);
        sem_wait(&provera);

        zbirNiza = 0;
        for (int j = 0; j < N; j++)
            zbirNiza += niz[j];

        if (zbirNiza > 2000)
            printf("zbirNiza: %d, zbir niza je VECI od 2000\n", zbirNiza);
        else
            printf("zbirNiza: %d, zbir niza je MANJI od 2000\n", zbirNiza);

        sem_post(&upis);
        sem_post(&upis);
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&upis);
    sem_destroy(&provera);

    return 0;
}
