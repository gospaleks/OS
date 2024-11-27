#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define LEN 256
#define N 5

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("neodgovarajuci broj argumenata\n");
        exit(1);
    }
    int linije[2];
    int brLinije[2];

    pipe(linije);
    pipe(brLinije);

    int pid = fork();

    if (pid != 0)
    {
        close(linije[0]);
        close(brLinije[0]);

        FILE *f = fopen(argv[1], "r");

        char linija[LEN];
        int redniBrLinije = 1;

        while (fgets(linija, LEN, f))
        {

            if (strlen(linija) > N)
            {
                write(linije[1], linija, LEN);
                write(brLinije[1], &redniBrLinije, sizeof(int));
            }
            redniBrLinije++;
        }

        fclose(f);

        close(linije[1]);
        close(brLinije[1]);

        wait(NULL);
    }
    else
    {
        close(linije[1]);
        close(brLinije[1]);

        char linija[LEN];
        int redniBrojLinije;

        while (read(linije[0], linija, LEN))
        {
            read(brLinije[0], &redniBrojLinije, sizeof(int));

            if (redniBrojLinije % 2 == 0)
            {
                printf("%s\n", linija);
                strcpy(linija, "");
            }
        }

        close(linije[0]);
        close(brLinije[0]);
    }
    return 0;
}