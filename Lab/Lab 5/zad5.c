#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define kljuc 12001

struct mymsgbuf
{
    long tip;
    int broj;
};

int main()
{
    struct mymsgbuf bafer;

    int msgid = msgget(kljuc, 0666 | IPC_CREAT);

    int pid = fork();

    if (pid != 0)
    {
        /* roditelj */

        do
        {
            scanf("%d", &bafer.broj);
            bafer.tip = 22;
            msgsnd(msgid, &bafer, sizeof(bafer) - sizeof(long), 0);
            msgrcv(msgid, &bafer, sizeof(bafer) - sizeof(long), 23, 0);

        } while (bafer.broj);

        wait(NULL);
        msgctl(msgid, IPC_RMID, NULL);
    }
    else
    {
        /* dete */

        int suma = 0;
        do
        {
            msgrcv(msgid, &bafer, sizeof(bafer) - sizeof(long), 22, 0);
            suma += bafer.broj;
            bafer.broj = suma;
            bafer.tip = 23;
            msgsnd(msgid, &bafer, sizeof(bafer) - sizeof(long), 0);

        } while (suma);

        printf("kraj... suma je %d\n", suma);
    }
    return 0;
}