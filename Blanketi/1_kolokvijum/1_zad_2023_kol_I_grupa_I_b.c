#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

sem_t glavna, ispis;

void *ispisNLinija(void *arg)
{
    int *n = (int *)arg;

    while (1)
    {
        sem_wait(&ispis);

        if (*n == 99)
            break;

        int pid = fork();
        if (pid == 0)
        {
            char nStr[100];
            sprintf(nStr, "%d", *n);
            char brLinija[100] = "-";
            strcat(brLinija, nStr);

            if (execlp("head", "head", brLinija, "data.txt", NULL) < 0)
            {
                printf("greska...\n");
                exit(1);
            }
        }
        else
        {
            wait(NULL);
            sem_post(&glavna);
        }
    }
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