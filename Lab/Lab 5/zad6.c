#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define LEN 128
#define KLJUC 12001

struct mymsgbuf
{
    long tip;
    char linija[LEN];
    int rbrLinije;
    int brPojavljivanja;
};

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Neodgovarajuci broj argumenata.\n");
        exit(1);
    }

    struct mymsgbuf buff;
    buff.tip = 1;

    int msqid = msgget(KLJUC, 0666 | IPC_CREAT);

    int pid = fork();
    if (pid != 0)
    {
        // roditelj

        FILE *f = fopen(argv[1], "r");
        if (f == 0)
            exit(1);

        while (fgets(buff.linija, LEN, f))
        {
            buff.tip = 1;
            msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);
            msgrcv(msqid, &buff, sizeof(buff) - sizeof(long), 2, 0); // prima tip 2 od deteta

            printf("Redni broj linije: %d\nBroj pojavljivanja velikih slova: %d\n\n", buff.rbrLinije,
                   buff.brPojavljivanja);
        }

        fclose(f);

        printf("KRAJ\n");
        buff.rbrLinije = -1; // flag za kraj
        buff.tip = 1;
        msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

        wait(NULL);
        msgctl(msqid, IPC_RMID, NULL);
    }
    else
    {
        // dete

        int br = 1;

        while (1)
        {
            // preuzmi liniju od roditelja
            msgrcv(msqid, &buff, sizeof(buff) - sizeof(long), 1, 0);

            // testiraj flag za kraj
            if (buff.rbrLinije == -1)
                break;

            int i = 0;
            int brVelikih = 0;

            while (buff.linija[i] != '\0')
            {
                if (buff.linija[i] >= 'A' && buff.linija[i] <= 'Z')
                    ++brVelikih;
                ++i;
            }

            buff.brPojavljivanja = brVelikih;
            buff.rbrLinije = br;

            // vrati rezultat
            buff.tip = 2;
            msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

            ++br;
        }
    }

    return 0;
}