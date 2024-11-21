#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char buffer[100];
int n, br;

sem_t citanje, pisanje;

void *pisiUFajl(void *arg)
{
    FILE *f = fopen((char *)arg, "w");
    if (f == 0)
        return NULL;

    while (1)
    {
        sem_wait(&pisanje);

        if (br < 1)
            break;

        // pisemo br elemenata (od 1B) jer je uvek to velicine n sem u poslednjem slucaju kad procita dole manje od n
        fwrite(buffer, 1, br, f);

        sem_post(&citanje);
    }
    return NULL;
}

// argv[1] - izvorisni fajl, argv[2] - N (br bajtova), argv[3] - odredisni fajl
int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Neodgovarajuci broj argumenata.\n");
        exit(1);
    }

    n = atoi(argv[2]);

    FILE *f = fopen(argv[1], "rb");
    if (f == 0)
    {
        printf("Greska pri otvaranju izvornog fajla.\n");
        exit(1);
    }

    pthread_t t;
    pthread_create(&t, NULL, (void *)pisiUFajl, (void *)argv[3]);

    sem_init(&citanje, 0, 1);
    sem_init(&pisanje, 0, 0);

    br = n;
    while (br > 0)
    {
        sem_wait(&citanje);

        // cita 1 element, velicine n, iz fajla f u buffer
        // funkcija vraca broj procitanih bajtova
        br = fread(buffer, 1, n, f);

        sem_post(&pisanje);
    }

    fclose(f);

    pthread_join(t, NULL);

    sem_destroy(&citanje);
    sem_destroy(&pisanje);

    return 0;
}