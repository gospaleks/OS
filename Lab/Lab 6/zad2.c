#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

struct fajl
{
    char imeFajla[256];
    int velicina;
};

struct fajl *niz;
int brTr;

void dodaj(struct fajl fajl)
{
    if (brTr < 5)
    {
        int i = brTr - 1;
        while (i >= 0 && niz[i].velicina > fajl.velicina)
        {
            niz[i + 1] = niz[i];
            i--;
        }
        niz[i + 1] = fajl;
        brTr++;
    }
    else
    {
        if (fajl.velicina < niz[4].velicina)
        {
            int i = 4;
            while (i > 0 && niz[i - 1].velicina > fajl.velicina)
            {
                niz[i] = niz[i - 1];
                i--;
            }
            niz[i] = fajl;
        }
    }
}

void obradi(char *imeDirektorijuma)
{
    DIR *dp;
    if ((dp = opendir(imeDirektorijuma)) == NULL)
    {
        printf("Greska prilikom otvaranja direktorijuma %s.\n", imeDirektorijuma);
        exit(1);
    }

    struct dirent *dirp;
    struct stat statbuf;
    char path[255];

    while ((dirp = readdir(dp)) != NULL)
    {
        // Preskoci ., .. da se ne upetlja
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;

        strcpy(path, imeDirektorijuma);
        strcat(path, "/");
        strcat(path, dirp->d_name);

        int result;
        if ((result = stat(path, &statbuf)) == -1)
        {
            printf("Neuspesno citanje podataka o objektu: %s\n", dirp->d_name);
            continue;
        }

        if (S_ISDIR(statbuf.st_mode))
            obradi(path);
        else if (S_ISREG(statbuf.st_mode))
        {
            // printf("%s -> %s (%ld)\n", imeDirektorijuma, dirp->d_name, statbuf.st_size);
            struct fajl fajl;
            strcpy(fajl.imeFajla, dirp->d_name);
            fajl.velicina = statbuf.st_size;
            dodaj(fajl);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Neodgovarajuci broj argumenta.\n");
        exit(1);
    }

    niz = (struct fajl *)malloc(sizeof(struct fajl) * 5);

    obradi(argv[1]);

    for (int i = 0; i < brTr; ++i)
        printf("%s, %d\n", niz[i].imeFajla, niz[i].velicina);

    free(niz);

    return 0;
}