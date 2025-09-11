#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define SIZE 4

int buffer[SIZE];
int suma = 0;

sem_t drugaNit;
sem_t prvaNit;

void *producer(void *arg)
{
    (void)arg;

    while (1)
    {
        sem_wait(&drugaNit);

        if (suma > 250)
            break;

        for (int i = 0; i < SIZE; ++i)
            buffer[i] = rand() % 100; /* 0..99 */

        sem_post(&prvaNit);
    }

    return NULL;
}

int main(void)
{
    srand((unsigned)time(NULL));

    sem_init(&drugaNit, 0, 1);
    sem_init(&prvaNit, 0, 0);

    pthread_t prod;
    pthread_create(&prod, NULL, producer, NULL);

    while (1)
    {
        sem_wait(&prvaNit);
        suma = 0;

        for (int i = 0; i < SIZE; ++i)
            suma += buffer[i];

        if (suma < 50)
            printf("Zbir skupa = %d -> Zbir je MANJI od 50\n", suma);
        else
            printf("Zbir skupa = %d -> Zbir je VECA ili JEDNAK 50\n", suma);

        if (suma > 250)
        {
            sem_post(&drugaNit);
            break;
        }

        sem_post(&drugaNit);
    }

    pthread_join(prod, NULL);
    sem_destroy(&drugaNit);
    sem_destroy(&prvaNit);

    printf("Kraj: kumulativni zbir > 250 (=%d)\n", suma);
    return 0;
}
