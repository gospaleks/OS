#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <unistd.h>

#define LEN 256

// argv[1] - izvorni fajl, argv[2] - odredisni fajl
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Neodgovarajuci broj argumenata.\n");
        exit(1);
    }

    int pd[2];
    pipe(pd);

    int pid = fork();
    if (pid != 0)
    {
        // roditelj
        close(pd[0]); // zatvori kraj za citanje jer pisemo

        FILE *f = fopen(argv[1], "r");
        if (f == 0)
            exit(1);

        char buff[LEN];
        while (fgets(buff, LEN, f))
            write(pd[1], buff, LEN);

        fclose(f);

        close(pd[1]);
    }
    else
    {
        // dete
        close(pd[1]); // zatvaramo kraj za pisanje jer citamo

        // mapiramo standardni izlaz na strim tekstualne datoteke
        int fd = open(argv[2], O_CREAT | O_TRUNC | O_RDWR, 0666);
        dup2(fd, 1);

        char buff[LEN];
        while (read(pd[0], buff, LEN))
        {
            int n = strlen(buff);
            for (int i = 1; i < n; ++i) // poslednji karakter ispadne al bmk ionako je glupo
                printf("%c", buff[i - 1]);
        }

        close(fd);

        close(pd[0]);
    }

    return 0;
}