#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>

/* nije receno kako da se resi sinhronizacija pa sam ja resio koriscenjem semafora */

sem_t sedam, ostali;

void *deljiviSedam(void *arg)
{
    int n = *((int *)arg);

    for (int i = 7; i <= n; i += 7)
    {
        sem_wait(&sedam);
        printf("%d! ", i);
        sem_post(&ostali);
    }
}

void *sviOstali(void *arg)
{
    int n = *((int *)arg);

    for (int i = 0; i <= n; i++)
    {
        if (i == 0) // i%7=0 za i=0 tako da mora i ovaj uslov
        {
            sem_wait(&ostali);
            printf("%d ", i);
            sem_post(&ostali);
        }
        else if (i % 7 != 0)
        {
            sem_wait(&ostali);
            printf("%d ", i);
            sem_post(&ostali);
        }
        else
        {
            sem_post(&sedam);
            sem_wait(&ostali);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("neispravan broj argumenata!\n");
        return 1;
    }

    int n = atoi(argv[1]);

    sem_init(&sedam, 0, 0);
    sem_init(&ostali, 0, 1);

    pthread_t t1, t2;

    pthread_create(&t1, NULL, (void *)deljiviSedam, &n);
    pthread_create(&t2, NULL, (void *)sviOstali, &n);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&sedam);
    sem_destroy(&ostali);

    printf("\n");

    return 0;
}