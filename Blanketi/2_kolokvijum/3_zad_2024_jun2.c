#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
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
    struct mymsgbuf buff;
    buff.tip = 1;

    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    int pid = fork();

    if (pid != 0)
    {
        do
        {
            printf("Unesi broj: ");
            scanf("%d", &buff.broj);

            if ((buff.broj >= 100 && buff.broj <= 999) || buff.broj == 0)
                msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);
            else
                printf("Unositi samo trocifrene brojeve.\n");

        } while (buff.broj != 0);

        wait(NULL);
        msgctl(msqid, IPC_RMID, NULL);
    }
    else
    {
        FILE *f = fopen("sumaCifara.txt", "w");

        while (1)
        {
            msgrcv(msqid, &buff, sizeof(buff) - sizeof(long), 1, 0);

            if (buff.broj == 0)
                break;

            int tmp = buff.broj;
            int suma = 0;
            while (tmp != 0)
            {
                suma += tmp % 10;
                tmp /= 10;
            }

            fprintf(f, "%d -> %d\n", buff.broj, suma);
        }

        fclose(f);
    }
    return 0;
}
