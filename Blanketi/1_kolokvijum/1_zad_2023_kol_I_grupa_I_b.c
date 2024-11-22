#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LEN 128

sem_t glavna, ispis;

void *ispisNLinija(void *arg)
{
    int *n = (int *)arg;
    char linija[MAX_LEN];

    FILE *f = fopen("data.txt", "r");

    while (1)
    {
        sem_wait(&ispis);

        if (*n == 99)
            break;

        // rewind vraca kursor citanja na pocetak fajla
        // ekvivalent je fseek(f, 0, SEEK_SET);
		// jos jedna solucija je da se fajl otvara i zatvara svaki put
        rewind(f);

        int i = 0;
        while (i < *n && !feof(f))
        {
            fgets(linija, MAX_LEN, f);
            printf("%s", linija);
            sleep(1);
            ++i;
        }

        sem_post(&glavna);
    }

    fclose(f);

    return NULL;
}

int main()
{
    int n;

    sem_init(&glavna, 0, 1);
    sem_init(&ispis, 0, 0);

    pthread_t t;
    pthread_create(&t, NULL, (void *)ispisNLinija, &n);

    do
    {
        sem_wait(&glavna);

        printf("\nuneti N:\n");
        scanf("%d", &n);

        sem_post(&ispis);

    } while (n != 99);

    pthread_join(t, NULL);

    sem_destroy(&glavna);
    sem_destroy(&ispis);

    return 0;
}