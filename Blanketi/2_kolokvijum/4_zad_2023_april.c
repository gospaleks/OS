#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

void obradi(char *direktorijum, int nivo, char **imeNajveceDatoteke, int *velicinaNajveceDatoteke)
{
    // Do nivoa 5
    if (nivo == 5)
        return;

    DIR *dp;
    if ((dp = opendir(direktorijum)) == NULL)
    {
        printf("Greska prilikom otvaranja direktorijuma.\n");
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
            // Svaki poziv povecava nivo
            obradi(path, nivo + 1, imeNajveceDatoteke, velicinaNajveceDatoteke);
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            if (statbuf.st_size > *velicinaNajveceDatoteke)
            {
                *velicinaNajveceDatoteke = statbuf.st_size;
                strcpy(*imeNajveceDatoteke, path);
            }
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

    // U ove promenljive ce f-ja obradi da upise rezultat
    char *imeNajveceDatoteke = (char *)malloc(255 * sizeof(char));
    int velicinaNajveceDatoteke = -1;

    obradi(argv[1], 0, &imeNajveceDatoteke, &velicinaNajveceDatoteke);
    printf("Najveca datoteka: %s - %d B\n", imeNajveceDatoteke, velicinaNajveceDatoteke);

    free(imeNajveceDatoteke);

    return 0;
}