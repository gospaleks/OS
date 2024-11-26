#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define N 20

sem_t uNiz, naKonzolu;

int niz[N];

void *popunjavanjeNiza(void *arg)
{
    int n = *((int *)arg);

    for (int i = 0; i < n; i++)
    {
        sem_wait(&uNiz);
        for (int j = 0; j < N; j++)
            niz[j] = rand() % 101;
        sem_post(&naKonzolu);
    }
    return NULL;
}

void *sumaNiza(void *arg)
{
    int n = *((int *)arg);
    for (int i = 0; i < n; i++)
    {
        int suma = 0;
        sem_wait(&naKonzolu);
        for (int j = 0; j < N; j++)
        {
            suma += niz[i];
            printf("%d ", niz[j]);
        }
        printf("\n--- suma je: %d\n", suma);
        sem_post(&uNiz);
    }
    return NULL;
}

int main()
{
    srand(time(NULL));

    int n;
    printf("uneti n:\n");
    scanf("%d", &n);

    sem_init(&uNiz, 0, 1);
    sem_init(&naKonzolu, 0, 0);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, (void *)popunjavanjeNiza, &n);
    pthread_create(&t2, NULL, (void *)sumaNiza, &n);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&uNiz);
    sem_destroy(&naKonzolu);

    return 0;
}