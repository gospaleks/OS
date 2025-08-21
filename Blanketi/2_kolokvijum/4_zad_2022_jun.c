#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void obradi(char *direktorijum, int nivo)
{
    if (nivo > 3)
        return;

    DIR *dp;
    if ((dp = opendir(direktorijum)) == NULL)
    {
        printf("Greska prilikom otvaranja direktorijuma.\n");
        exit(1);
    }

    struct dirent *dirp;
    struct stat statbuf;
    char path[1024];

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
            obradi(path, nivo + 1);
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            if (strstr(dirp->d_name, "log") && statbuf.st_size > 15)
            {
                // Kreiraj novi proces jer execlp pregazi trenutni proces
                // Znaci kreira se novi proces i execlp ga pregazi, obrise, a roditelj nastavi posle brisanja
                int pid = fork();
                if (pid == 0)
                {
                    if (execlp("rm", "rm", path, NULL) < 0)
                    {
                        printf("greska\n");
                        exit(1);
                    }
                }
                else
                {
                    wait(NULL);
                }
            }
        }
    }

    closedir(dp);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Neodgovarajuci broj parametara.");
        exit(1);
    }

    obradi(argv[1], 0);

    printf("Uspesno zavrsena obrada.\n");

    return 0;
}