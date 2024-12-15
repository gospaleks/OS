#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define KLJUC 12010

struct mymsgbuf
{
    long tip;
    char karakter;
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
            scanf(" %c", &bafer.karakter);
            msgsnd(msqid, &bafer, sizeof(bafer) - sizeof(long), 0);

        } while (bafer.karakter < '0' || bafer.karakter > '9');

        wait(NULL);
        msgctl(msqid, IPC_RMID, NULL);
    }
    else
    {
        // dete

        FILE *f = fopen("karakteri.txt", "w");

        if (f == NULL)
        {
            printf("problem sa otvaranjem fajla");
            exit(1);
        }

        while (1)
        {
            msgrcv(msqid, &bafer, sizeof(bafer) - sizeof(long), 4, 0);

            if (bafer.karakter >= '0' && bafer.karakter <= '9')
                break;

            fprintf(f, "%c\n", bafer.karakter);
        }

        fclose(f);
    }
    return 0;
}