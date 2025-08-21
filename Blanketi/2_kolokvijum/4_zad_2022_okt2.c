#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

void obradi(char *direktorijum, char *nazivDatoteke, char **lokacija, int *velicina)
{
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
            obradi(path, nazivDatoteke, lokacija, velicina);
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            if (strcmp(dirp->d_name, nazivDatoteke) == 0)
            {
                strcpy(*lokacija, path); // u lokacija kopiramo celu putanju pronadjenog fajla
                *velicina = statbuf.st_size;
                return;
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Neodgovarajuci broj parametara.");
        exit(1);
    }

    char *lokacija = (char *)malloc(255 * sizeof(char));
    int velicina = -1;

    obradi(argv[1], argv[2], &lokacija, &velicina);

    printf("Fajl pronadjen na lokaciji: %s, velicina: %d\n", lokacija, velicina);

    free(lokacija);

    return 0;
}
