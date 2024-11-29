#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define N 2
#define M 10

int niz[2];

sem_t generisi, proveri;

void *upisivanjeNiza(void *arg)
{
    for (int i = 0; i < M; i++)
    {
        sem_wait(&generisi);

        niz[0] = rand() % 100 + 100;
        niz[1] = rand() % 100 + 100;

        sem_post(&proveri);
    }
    return NULL;
}

int main()
{
    srand(time(NULL));

    sem_init(&generisi, 0, 1);
    sem_init(&proveri, 0, 0);

    pthread_t t;
    pthread_create(&t, NULL, (void *)upisivanjeNiza, NULL);

    for (int i = 0; i < M; i++)
    {
        sem_wait(&proveri);

        if (niz[0] == niz[1])
            printf("jednaki su\n");
        else
            printf("nisu jednaki\n");

        sem_post(&generisi);
    }

    pthread_join(t, NULL);

    sem_destroy(&generisi);
    sem_destroy(&proveri);

    return 0;
}