#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define LEN 256

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Neodgovarajuci broj argumenata.\n");
        exit(1);
    }

    int pd1[2]; // prema detetu
    int pd2[2]; // prema roditelju

    pipe(pd1);
    pipe(pd2);

    int pid = fork();

    if (pid == 0)
    {
        close(pd1[1]);
        close(pd2[0]);

        char linija[LEN];
        char odgovor[LEN];
        while (read(pd1[0], linija, LEN))
        {
            // karakter je u argv[2]
            if (strstr(linija, argv[2]))
                strcpy(odgovor, "FOUND");
            else
                strcpy(odgovor, "NOT FOUND");

            write(pd2[1], odgovor, sizeof(odgovor));
        }

        close(pd2[1]);
        close(pd1[0]);
    }
    else
    {
        close(pd1[0]);
        close(pd2[1]);

        FILE *f = fopen(argv[1], "r");
        if (f == 0)
            exit(1);

        char linija[LEN];
        char odgovor[LEN];

        while (fgets(linija, LEN, f))
        {
			// Posalje detetu, on obradi i odma prihvataj odgovor
            write(pd1[1], linija, LEN);
            read(pd2[0], odgovor, sizeof(odgovor));
            printf("Poruka: %s\n", odgovor);
        }

        fclose(f);

        close(pd1[1]);
        close(pd2[0]);

        wait(NULL);
    }

    return 0;
}