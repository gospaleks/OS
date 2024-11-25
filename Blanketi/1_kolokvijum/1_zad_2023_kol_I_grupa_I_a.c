#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

sem_t glavni, sporedni;

void *fja(void *arg)
{
    int *n = (int *)arg;

    // Upis u fajl
    sem_wait(&sporedni);

    FILE *f = fopen("data.txt", "w");
    if (f == 0)
        exit(1);

    fprintf(f, "%d\n", *n);
    for (int i = 0; i < *n; ++i)
        fprintf(f, "%c\n", rand() % 26 + 97); // samo a-z

    fclose(f);

    sem_post(&glavni);

    // Odbrojavanje
    while (1)
    {
        sem_wait(&sporedni);

        if (*n == 99)
            break;

        int br = (*n) + 1;
        while (br--)
        {
            printf("Odbrojavam... %d\n", br);
            sleep(1);
        }

        sem_post(&glavni);
    }

    sem_post(&glavni);
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    int n;

    sem_init(&glavni, 0, 0);
    sem_init(&sporedni, 0, 0);

    pthread_t t;
    pthread_create(&t, NULL, (void *)fja, (void *)&n);

    printf("Unesite N za upis u data.txt: ");
    scanf("%d", &n);
    sem_post(&sporedni);

    while (n != 99)
    {
        sem_wait(&glavni);
        printf("Unesite N za odbrojavanje: ");
        scanf("%d", &n);
        sem_post(&sporedni);
    }

    pthread_join(t, NULL);

    sem_destroy(&glavni);
    sem_destroy(&sporedni);

    return 0;
}