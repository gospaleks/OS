#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define LEN 256

sem_t s;
char recenica[LEN];

// Mora pointeri da se swapuju
void swapStrings(char **s1, char **s2)
{
    char *tmp = *s1;
    *s1 = *s2;
    *s2 = tmp;
}

void *sortiraj(void *arg)
{
    // Izbroj reci
    int n = 0;
    int i = 0;
    while (recenica[i] != '\0')
    {
        if (recenica[i] == ' ')
            ++n;
        ++i;
    }
    ++n;

    // Smesti reci u niz
    char **reci;
    reci = (char **)malloc(sizeof(char *) * n);

    char *token = strtok(recenica, " ");

    i = 0;
    while (token != NULL)
    {
        reci[i] = (char *)malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(reci[i], token);
        token = strtok(NULL, " ");
        i++;
    }

    // Sortiraj reci
    for (int i = 0; i < n - 1; ++i)
    {
        int minInd = i;
        for (int j = i + 1; j < n; ++j)
            if (strcmp(reci[j], reci[minInd]) < 0)
                minInd = j;

        swapStrings(&reci[i], &reci[minInd]);
    }

    // Spoji sortirano u recenicu
    strcpy(recenica, "");
    for (int i = 0; i < n; ++i)
    {
        strcat(recenica, reci[i]);
        if (i != n - 1)
            strcat(recenica, " ");
    }

    // Oslobodi mem
    for (int i = 0; i < n; ++i)
        free(reci[i]);
    free(reci);

    sem_post(&s); // Aktiviraj obradu
}

void *stampajUredjeno(void *arg)
{
    sem_wait(&s);

    int n = strlen(recenica);
    recenica[0] = toupper(recenica[0]);
    if (recenica[n - 1] != '.')
        strcat(recenica, ".");

    printf("%s\n", recenica);
}

int main(int argc, char **argv)
{
    sem_init(&s, 0, 0);

    printf("Unesite recenicu:\n");
    gets(recenica); // da se izbegne warning koristi fgets(recenica, LEN, stdin)

    pthread_t t1, t2;
    pthread_create(&t1, NULL, (void *)sortiraj, NULL);
    pthread_create(&t2, NULL, (void *)stampajUredjeno, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&s);

    printf("KRAJ\n");

    return 0;
}