#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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
        // Preskoci ., .. da se ne upetlja (tacka i tackatacka)
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
            char *ime = dirp->d_name;
            int n = strlen(ime);

            if (ime[n - 1] == 'h' && ime[n - 2] == 's' && ime[n - 3] == '.')
            {
                int pid = fork();
                if (pid == 0)
                {
                    printf("Izvrsavam: %s\n", ime);
                    if (execlp("sh", "sh", path, NULL) < 0)
                        exit(1);
                }
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

    obradi(argv[1]);

    return 0;
}