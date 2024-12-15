#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define KLJUC 12010
#define MAX_LEN 256

struct mymsgbuf
{
    long tip;
    char linija[MAX_LEN]
};

int main()
{
    struct mymsgbuf bafer;
    bafer.tip = 4;

    int msqid = msgget(KLJUC, 0666 | IPC_CREAT);

    int pid = fork();

    if (pid != 0)
    {
        // roditelj

        do
        {
            gets(bafer.linija);
            msgsnd(msqid, &bafer, sizeof(bafer) - sizeof(long), 0);

        } while (strcmp(bafer.linija, "KRAJ") != 0);

        wait(NULL);
        msgctl(msqid, IPC_RMID, NULL);
    }
    else
    {
        // dete

        FILE *f = fopen("izlaz.txt", "w");

        if (f == NULL)
        {
            printf("problem sa otvaranjem fajla");
            exit(1);
        }

        while (1)
        {
            msgrcv(msqid, &bafer, sizeof(bafer) - sizeof(long), 4, 0);

            if (strcmp(bafer.linija, "KRAJ") == 0)
                break;

            fprintf(f, "%s\n", bafer.linija);
        }

        fclose(f);
    }
    return 0;
}