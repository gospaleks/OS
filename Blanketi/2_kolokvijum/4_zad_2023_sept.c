#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void obradi(char *direktorijum, char *nstr, char *mstr)
{
    DIR *dp;
    if ((dp = opendir(direktorijum)) == NULL)
    {
        printf("Greska prilikom otvaranja direktorijuma.\n");
        exit(1);
    }

    int n = atoi(nstr);

    char brLinija[10];
    strcpy(brLinija, "-");
    strcat(brLinija, mstr);

    struct dirent *dirp;
    struct stat statbuf;

    int i = 0;
    while (i < n && (dirp = readdir(dp)) != NULL)
    {
        int result;
        if ((result = stat(dirp->d_name, &statbuf)) == -1)
        {
            printf("Neuspesno citanje podataka o objektu: %s\n", dirp->d_name);
            continue;
        }

        if (S_ISREG(statbuf.st_mode))
        {
            int pid = fork();
            if (pid == 0)
            {
                if (execlp("head", "head", brLinija, dirp->d_name, NULL) < 0)
                    exit(1);
                return;
            }
            else
            {
                wait(NULL);
                ++i;
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Neodgovarajuci broj argumenata.\n");
        exit(1);
    }

    obradi(argv[1], argv[2], argv[3]);

    return 0;
}
