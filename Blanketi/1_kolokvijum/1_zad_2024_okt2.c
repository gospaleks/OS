#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

sem_t par, nepar;

typedef struct argumenti
{
    FILE *pisanje;
    char *citanje;
} argumenti;

void *neparneLinije(void *arg)
{
    argumenti ar = *((argumenti *)arg);

    FILE *f = fopen(ar.citanje, "r");

    if (f == NULL)
    {
        printf("problem sa otvaranjem fajla u prvoj niti\n");
        exit(1);
    }

    char linija[256];
    int brojLinije = 1;

    while (fgets(linija, sizeof(linija), f))
    {
        if (brojLinije % 2 != 0)
        {
            sem_wait(&nepar);
            fprintf(ar.pisanje, "%s\n", linija);
            sem_post(&par);
        }
        brojLinije++;
    }

    fclose(f);
    return NULL;
}

void *parneLinije(void *arg)
{
    argumenti ar = *((argumenti *)arg);

    FILE *f = fopen(ar.citanje, "r");

    if (f == NULL)
    {
        printf("problem sa otvaranjem fajla u drugoj niti\n");
        exit(1);
    }

    char linija[256];
    int brojLinije = 1;

    while (fgets(linija, sizeof(linija), f))
    {
        if (brojLinije % 2 == 0)
        {
            sem_wait(&par);
            fprintf(ar.pisanje, "%s\n", linija);
            sem_post(&nepar);
        }
        brojLinije++;
    }

    fclose(f);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("neodgovarajuci broj argumenata\n");
        exit(1);
    }

    FILE *upis = fopen("zbir.txt", "w");

    if (upis == NULL)
    {
        printf("greska prilikom ovaranja zbir.txt\n");
        exit(1);
    }

    sem_init(&par, 0, 0);
    sem_init(&nepar, 0, 1);

    argumenti ar1;
    ar1.pisanje = upis;
    ar1.citanje = (char *)malloc((strlen(argv[1]) + 1) * sizeof(char));
    strcpy(ar1.citanje, argv[1]);

    argumenti ar2;
    ar2.pisanje = upis;
    ar2.citanje = (char *)malloc((strlen(argv[2]) + 1) * sizeof(char));
    strcpy(ar2.citanje, argv[2]);

    pthread_t t1, t2;

    pthread_create(&t1, NULL, (void *)parneLinije, &ar1);
    pthread_create(&t2, NULL, (void *)neparneLinije, &ar2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    fclose(upis);

    sem_destroy(&par);
    sem_destroy(&nepar);

    free(ar1.citanje);
    free(ar2.citanje);
}