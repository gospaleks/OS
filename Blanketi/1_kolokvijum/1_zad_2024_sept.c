#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t glavna, druga;

void *odbrojavaj(void *arg)
{
    int *vrednost = (int *)arg;

    while (1)
    {
        sem_wait(&druga);

        if (*vrednost == 999)
            break;

        for (int i = 0; i <= *vrednost; i++)
            printf("%d\t", i);
        printf("\n");

        sem_post(&glavna);
    }
    return NULL;
}

int main()
{
    int vrednost;
    sem_init(&glavna, 0, 1);
    sem_init(&druga, 0, 0);

    pthread_t t;
    pthread_create(&t, NULL, (void *)odbrojavaj, &vrednost);

    do
    {
        sem_wait(&glavna);

        printf("upisati vrednost ili 999 za izlazak:\n");
        scanf("%d", &vrednost);

        sem_post(&druga);

    } while (vrednost != 999);

    pthread_join(t, NULL);

    sem_destroy(&glavna);
    sem_destroy(&druga);

    return 0;
}