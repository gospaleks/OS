#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 10

pthread_mutex_t lock;
pthread_cond_t cond;
int mozeDrugaPolovina = 1;
int brojUpisanih = 0;

void stampajNiz(int *niz, char *poruka)
{
    printf("niz %s\n", poruka);
    for (int i = 0; i < N; i++)
        printf("%d ", niz[i]);
    printf("\n\n");
}

void sortirajNiz(int *niz, int odIndeks, int doIndeks)
{
    for (int i = odIndeks; i < doIndeks - 1; i++)
    {
        int min = i;
        for (int j = i; j < doIndeks; j++)
            if (niz[j] < niz[min])
                min = j;
        int pom = niz[i];
        niz[i] = niz[min];
        niz[min] = pom;
    }
}

void *prvaPolovinaSort(void *arg)
{
    int *niz = (int *)arg;

    sortirajNiz(niz, 0, N / 2);

    // stampajNiz(niz, "posle PRVE polovine");

    pthread_mutex_lock(&lock);

    brojUpisanih++;

    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&lock);
}

void *drugaPolovinaSort(void *arg)
{
    int *niz = (int *)arg;

    sortirajNiz(niz, N / 2, N);

    // stampajNiz(niz, "posle DRUGE polovine");

    pthread_mutex_lock(&lock);

    brojUpisanih++;

    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&lock);
}

int main()
{
    srand(time(NULL));

    int niz[N];

    for (int i = 0; i < N; i++)
        niz[i] = rand() % 11;

    stampajNiz(niz, "BEZ promena");

    pthread_t t1, t2;

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&t1, NULL, (void *)prvaPolovinaSort, (void *)niz);
    pthread_create(&t2, NULL, (void *)drugaPolovinaSort, (void *)niz);

    pthread_mutex_lock(&lock);

    while (brojUpisanih < 2)
    {
        pthread_cond_wait(&cond, &lock);
    }

    stampajNiz(niz, "na kraju:");

    pthread_mutex_unlock(&lock);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    return 0;
}