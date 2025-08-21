#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define KEY 10101

struct mymsgbuf
{
    long tip;
    int broj;
};

int main()
{
    srand(time(NULL));

    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    struct mymsgbuf buff;

    int pid = fork();

    if (pid != 0)
    {
        // roditelj

        while (1)
        {
            buff.broj = rand() % 1000;

            if (buff.broj < 10)
                buff.tip = 1;
            else if (buff.broj < 100)
                buff.tip = 2;
            else
                buff.tip = 3;

            msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);
            msgrcv(msqid, &buff, sizeof(buff) - sizeof(long), 4, 0);

            if (buff.broj == -1)
                break;
        }

        wait(NULL);
        msgctl(msqid, IPC_RMID, NULL);
    }
    else
    {
        // dete

        int suma1 = 0, suma2 = 0, suma3 = 0;

        while (suma1 + suma2 + suma3 <= 50000)
        {
            msgrcv(msqid, &buff, sizeof(buff) - sizeof(long), 0, 0);

            if (buff.tip == 1)
                suma1 += buff.broj;
            else if (buff.tip == 2)
                suma2 += buff.broj;
            else
                suma3 += buff.broj;

            buff.tip = 4;
            msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);
        }

        printf("suma1: %d, suma2: %d, suma3: %d, ukupno: %d\n", suma1, suma2, suma3, suma1 + suma2 + suma3);

        buff.tip = 4;
        buff.broj = -1;
        msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);
    }

    return 0;
}
