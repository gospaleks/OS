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

        int broj;
        for (int i = 0; i < 10; i++)
        {
            while (1)
            {
                scanf("%d", &bafer.broj);

                if (bafer.broj < 10)
                    printf("uneti samo visecifrene brojeve\n");
                else
                    break;
            }

            msgsnd(msqid, &bafer, sizeof(bafer) - sizeof(long), 0);
        }

        wait(NULL);
        msgctl(msqid, IPC_RMID, NULL);
    }
    else
    {
        // dete

        int suma;
        for (int i = 0; i < 10; i++)
        {
            msgrcv(msqid, &bafer, sizeof(bafer) - sizeof(long), 1, 0);

            int tmp = bafer.broj;
            int suma = 0;
            while (tmp != 0)
            {
                suma += tmp % 10;
                tmp /= 10;
            }
            printf("interacija: %d\tsuma: %d\n", i + 1, suma);
        }
    }
}