#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// VERZIJA 1 - manje optimalna (objasnjeno u v2 zasto) 

void traziDatoteku(char* direktorijum, char* ime, int velicina)
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
            traziDatoteku(path,ime, velicina);
        }

        else if (S_ISREG(statbuf.st_mode))
        {
            if (strcmp(ime, dirp->d_name) == 0 && (velicina == statbuf.st_size))
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

    DIR *dp;
    if ((dp = opendir(argv[1])) == NULL)
    {
        printf("Greska prilikom otvaranja prvog direktorijuma.\n");
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

        strcpy(path, argv[1]);
        strcat(path, "/");
        strcat(path, dirp->d_name);

        int result;
        if ((result = stat(path, &statbuf)) == -1)
        {
            printf("Neuspesno citanje podataka o objektu: %s\n", dirp->d_name);
            continue;
        }

        if (S_ISREG(statbuf.st_mode))
        {    
            traziDatoteku(argv[2], dirp->d_name, statbuf.st_size);
        }
    }

    return 0;
}
