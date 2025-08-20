#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define KEY 10101
#define MAX_LEN 256

struct mymsgbuf
{
    long tip;
    char line[MAX_LEN];
    int rbrLinije;
    int brVelikihSlova;
};

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Neodgovarajuci broj argumenata.\n");
        exit(1);
    }

    struct mymsgbuf buff;

    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    int pid = fork();
    if (pid != 0)
    {
        // roditelj
        FILE *f = fopen(argv[1], "r");
        if (f == 0)
            exit(1);

        while (1)
        {
            if (fgets(buff.line, MAX_LEN, f) == 0)
                break;

            // Detetu salje tip 1
            buff.tip = 1;
            msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

            // Dete mu salje tip 2
            msgrcv(msqid, &buff, sizeof(buff) - sizeof(long), 2, 0);

            printf("linija %d: %d\n", buff.rbrLinije, buff.brVelikihSlova);
        }

        // -1 salje da dete zna kad da prekine
        buff.tip = 1;
        buff.brVelikihSlova = -1;
        msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

        wait(NULL);

        msgctl(msqid, IPC_RMID, NULL);
    }
    else
    {
        int trRbrLinije = 0;
        while (1)
        {
            msgrcv(msqid, &buff, sizeof(buff) - sizeof(long), 1, 0);

            if (buff.brVelikihSlova == -1)
                break;

            buff.rbrLinije = trRbrLinije;
            buff.brVelikihSlova = 0;

            int len = strlen(buff.line);
            for (int i = 0; i < len; ++i)
                if (isupper(buff.line[i]))
                    ++buff.brVelikihSlova;

            // Salje roditelju rezultat obrade kao tip 2
            buff.tip = 2;
            msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

            ++trRbrLinije;
        }
    }

    return 0;
}