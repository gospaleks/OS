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

struct mymsgbuf
{
    long tip;
    int broj;
};

int main()
{
    struct mymsgbuf bafer;
    bafer.tip = 1;
    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    int pid = fork();

    if (pid != 0)
    {
        // roditelj
        for (int i = 0; i < 10; i++)
        {
            scanf("%d", &bafer.broj);
            msgsnd(msqid, &bafer, sizeof(bafer) - sizeof(long), 0);
        }

        wait(NULL);
        msgctl(msqid, IPC_RMID, NULL);
    }
    else
    {
        // dete
        for (int i = 0; i < 10; i++)
        {
            msgrcv(msqid, &bafer, sizeof(bafer) - sizeof(long), 1, 0);

            int zbir = 0;
            while (bafer.broj != 0)
            {
                zbir += bafer.broj % 10;
                bafer.broj /= 10;
            }

            printf("zbir cifara: %d\n", zbir);
        }
    }

    return 0;
}