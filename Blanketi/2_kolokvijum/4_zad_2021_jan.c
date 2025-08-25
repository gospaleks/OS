#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// N - velicina u bajtovima
// M - duzina apsolutne putanje u karakterima

void obradi(char* direktorijum, int N, int M)
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
            obradi(path, N, M);
        }
        else if (S_ISREG(statbuf.st_mode))
        {    
            int m = strlen(path);
            long long brojBajtova = statbuf.st_size;

            if (m > M && brojBajtova > N)
                printf("%s: %lldB\n", path, brojBajtova);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("neodgovarajuci broj argumenata\n");
        exit(1);
    }

    obradi(argv[1], atoi(argv[2]), atoi(argv[3]));

    return 0;
}