#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define LEN 100

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Neodgovarajuci broj argumenata.\n");
        exit(1);
    }

    char buff[LEN];

    int pd[2];
    if (pipe(pd) < 0)
        exit(1);

    int pid = fork();
    if (pid == 0)
    {
        close(pd[1]);

        FILE *f = fopen(argv[2], "w");
        if (f == 0)
            exit(1);

        while (read(pd[0], buff, LEN))
            fprintf(f, "%s", buff);

        fclose(f);
        close(pd[0]);
    }
    else
    {
        close(pd[0]);

        FILE *f = fopen(argv[1], "r");
        if (f == 0)
            exit(1);

        while (fgets(buff, LEN, f))
            write(pd[1], buff, LEN);

        fclose(f);
        close(pd[1]);

        wait(NULL);
    }

    return 0;
}