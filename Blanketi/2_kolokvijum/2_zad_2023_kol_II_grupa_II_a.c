#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int main()
{
    DIR *dp;
    if ((dp = opendir(".")) == NULL)
    {
        printf("Greska prilikom otvaranja direktorijuma.\n");
        exit(1);
    }

    // Statistika
    int brPodfoldera = 0;
    int brLinkova = 0;
    int brPraznihRegularnih = 0;
    int brRegularnih = 0;

    struct dirent *dirp;
    struct stat statbuf;

    while ((dirp = readdir(dp)) != NULL)
    {
        // Preskoci ., ..
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;

        int result;
        if ((result = stat(dirp->d_name, &statbuf)) == -1)
        {
            printf("Neuspesno citanje podataka o objektu: %s\n", dirp->d_name);
            continue;
        }

        if (S_ISDIR(statbuf.st_mode))
            ++brPodfoldera;
        else if (S_ISLNK(statbuf.st_mode))
            ++brLinkova;
        else if (S_ISREG(statbuf.st_mode))
        {
            if (statbuf.st_size == 0)
                ++brPraznihRegularnih;
            else
                ++brRegularnih;
        }
    }

    printf("Broj podfoldera: %d\n", brPodfoldera);
    printf("Broj linkova: %d\n", brLinkova);
    printf("Broj praznih regularnih fajlova: %d\n", brPraznihRegularnih);
    printf("Broj regularnih fajlova: %d\n", brRegularnih);

    closedir(dp); // ovo obavezno!

    return 0;
}