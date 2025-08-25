#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_IME_FAJLA 256

// VERZIJA 2 - optimalnija

// ovo je optimizacija u smislu da se ne zove rekurzivna pretraga za svaki fajl iz prvog 
// direktorijuma nego se ucitaju svi fajlovi pa se onda to prosledjuje na jedan rekurzivan poziv
// u kome se svaki fajl drugog direktorijuma poredi sa nizom fajlova prvog... 
// svakako je broj poredjenja fajlova isti kao u prosloj verziji

// prosla verzija je radila tako da se u prvom direktorijumu prolazi i za svaki fajl
// se zove funkcija traziDatoteku u kojoj se prosledjuje naziv fajla i velicina pa se poredjuje za tu datoteku
// svaki fajl iz drugog direktorijum... broj poredjenja je isti ali broj poziva rekurzivne funkcije je daleko veci

typedef struct 
{
    char ime[MAX_IME_FAJLA];
    long int velicina;
} FajlInfo;

// prebrojava samo koliko ima fajlova da bi mogao da napravim dinamicki niz struktura 
// sa tacno toliko mesta koja kasnije popunjavam
int prebrojFajlove(char* direktorijum) 
{
    int brojFajlova = 0;

    DIR *dp;
    if ((dp = opendir(direktorijum)) == NULL)
    {
        printf("Greska prilikom otvaranja prvog direktorijuma radi prebrojavanja fajlova.\n");
        exit(1);
    }

    struct dirent *dirp;
    struct stat statbuf;
    char path[256];

    while ((dirp = readdir(dp)) != NULL)
    {
        // Preskoci ., ..
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;

        strcpy(path, direktorijum);
        strcat(path, "/");
        strcat(path, dirp->d_name);

        int result;
        if ((result = stat(path, &statbuf)) == -1)
        {
            printf("Neuspesno citanje podataka o objektu: %s\n", dirp->d_name);
            continue;
        }

        // deo koji se razlikuje
        if (S_ISREG(statbuf.st_mode))
        {    
            brojFajlova++;
        }
    }

    return brojFajlova;
}

// popunjava prosledjeni niz
void ucitajFajlove(char* direktorijum, FajlInfo* niz)
{
    int indeks = 0;

    DIR *dp;
    if ((dp = opendir(direktorijum)) == NULL)
    {
        printf("Greska prilikom otvaranja prvog direktorijuma radi prebrojavanja fajlova.\n");
        exit(1);
    }

    struct dirent *dirp;
    struct stat statbuf;
    char path[256];

    while ((dirp = readdir(dp)) != NULL)
    {
        // Preskoci ., ..
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;

        strcpy(path, direktorijum);
        strcat(path, "/");
        strcat(path, dirp->d_name);

        int result;
        if ((result = stat(path, &statbuf)) == -1)
        {
            printf("Neuspesno citanje podataka o objektu: %s\n", dirp->d_name);
            continue;
        }

        // deo koji se razlikuje
        if (S_ISREG(statbuf.st_mode))
        {    
            strcpy(niz[indeks].ime, dirp->d_name);
            niz[indeks++].velicina = statbuf.st_size;
        }
    }
}

void traziDatoteku(char* direktorijum, FajlInfo* niz, int brojFajlova)
{
    DIR *dp;
    if ((dp = opendir(direktorijum)) == NULL)
    {
        printf("Greska prilikom otvaranja drugog direktorijuma.\n");
        exit(1);
    }

    struct dirent *dirp;
    struct stat statbuf;
    char path[256];

    while ((dirp = readdir(dp)) != NULL)
    {
        // Preskoci ., ..
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;

        strcpy(path, direktorijum);
        strcat(path, "/");
        strcat(path, dirp->d_name);

        int result;
        if ((result = stat(path, &statbuf)) == -1)
        {
            printf("Neuspesno citanje podataka o objektu: %s\n", dirp->d_name);
            continue;
        }

        if (S_ISDIR(statbuf.st_mode))
        {
            traziDatoteku(path, niz, brojFajlova);
        }

        // deo koji se razlikuje
        else if (S_ISREG(statbuf.st_mode))
        {
            for (int i = 0; i < brojFajlova; i++)
                if (strcmp(niz[i].ime, dirp->d_name) == 0 && niz[i].velicina == statbuf.st_size)
                    printf("%s: %ldB\n", dirp->d_name, statbuf.st_size);
        }
    }
}

// argv[1] - prvi direktorijum (pretpostavljamo da nema poddirektorijume, a i da ima mi ih preskacemo)
// argv[2] - drugi direktorijum

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("neodgovarajuci broj argumenata\n");
        exit(1);
    }

    int brojFajlova = prebrojFajlove(argv[1]);

    FajlInfo* nizFajlova = malloc(brojFajlova * sizeof(FajlInfo));

    ucitajFajlove(argv[1], nizFajlova);

    traziDatoteku(argv[2], nizFajlova, brojFajlova);
    
    free(nizFajlova);

    return 0;
}