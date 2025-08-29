#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void obradi(char* direktorijum, int nivoDo, int nivoTr)
{
    if (nivoTr > nivoDo)
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
            obradi(path, nivoDo, nivoTr + 1);
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            int pid = fork();
            if (pid == 0)
            {
                printf("\n#####################\n%s:\n", dirp->d_name);
                execlp("head", "head", "-n", "10", path, (char*)NULL);
            }
            wait(NULL);
        }
    }

}

// cita uvek jos jedan fajl koji ne znam koji je

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Neodgovarajuci broj parametara.");
        exit(1);
    }

    obradi(argv[1], atoi(argv[2]), 0);

    printf("\nUspesno zavrsena obrada.\n");

    return 0;
}