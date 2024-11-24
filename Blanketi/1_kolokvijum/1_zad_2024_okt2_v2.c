#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define LEN 256

sem_t parni, neparni;
int kraj;

void *citajIzPrve(void *arg)
{
    char *imeFajla = (char *)arg;

    FILE *f = fopen(imeFajla, "r");
    if (f == 0)
        exit(1);

    char linija[LEN];
    int brTrLinije = 1;
    while (fgets(linija, LEN, f) != NULL)
    {
        sem_wait(&parni);

        if (kraj)
            break;

        printf("%s", linija);
        FILE *f1 = fopen("zbir.txt", "a");
        fprintf(f1, "%s (linija %d): %s", imeFajla, brTrLinije, linija);
        fclose(f1);

        ++brTrLinije;

        sem_post(&neparni);
    }

    kraj = 1;
    sem_post(&neparni);

    fclose(f);
}

void *citajIzDruge(void *arg)
{
    char *imeFajla = (char *)arg;

    FILE *f = fopen(imeFajla, "r");
    if (f == 0)
        exit(1);

    char linija[LEN];
    int brTrLinije = 1;
    while (fgets(linija, LEN, f) != NULL)
    {
        sem_wait(&neparni);

        if (kraj)
            break;

        printf("%s", linija);

        FILE *f1 = fopen("zbir.txt", "a");
        fprintf(f1, "%s (linija %d): %s", imeFajla, brTrLinije, linija);
        fclose(f1);

        ++brTrLinije;

        sem_post(&parni);
    }

    kraj = 1;
    sem_post(&parni);

    fclose(f);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Neodgovarajuci broj	argumenata.\n");
        exit(1);
    }

    // Da pregazi prethodno stanje
    FILE *f = fopen("zbir.txt", "w");
    fclose(f);

    sem_init(&parni, 0, 0);
    sem_init(&neparni, 0, 1);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, (void *)citajIzPrve, (void *)argv[1]);
    pthread_create(&t2, NULL, (void *)citajIzDruge, (void *)argv[2]);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&parni);
    sem_destroy(&neparni);

    return 0;
}