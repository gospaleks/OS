#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

void obradi(char *direktorijum, int nivo, char **imeNajmanje, int *velicinaNajmanje, int dubina)
{
    if (nivo == dubina)
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
        // preskocemo ., ..
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
            obradi(path, nivo + 1, imeNajmanje, velicinaNajmanje, dubina);
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            if (statbuf.st_size > 1024 &&
                strlen(path) > 4 &&
                strcmp(path + strlen(path) - 4, ".txt") == 0)
            {
                if (statbuf.st_size < *velicinaNajmanje)
                {
                    *velicinaNajmanje = statbuf.st_size;
                    strcpy(*imeNajmanje, path);
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Neodgovarajuci broj argumenta.\n");
        exit(1);
    }

    char *imeNajmanje = (char *)malloc(255 * sizeof(char));
    int velicinaNajmanje = INT_MAX;

    obradi(argv[1], 0, &imeNajmanje, &velicinaNajmanje, atoi(argv[2]));

    printf("\nNajmanja: %s\nVelicina: %d B\n\n", imeNajmanje, velicinaNajmanje);

    free(imeNajmanje);

    return 0;
}