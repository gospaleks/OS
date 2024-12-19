#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define KLJUC 10101

struct poruka_s
{
    long tip;
    int broj;
};

int main()
{
    int porukaId = msgget(KLJUC, 0666 | IPC_CREAT);

    struct poruka_s poruka;
    poruka.tip = 1;

    int pid = fork();

    if (pid != 0)
    {
        // roditelj

        do
        {
            scanf("%d", &poruka.broj);

            if (poruka.broj < 0 || poruka.broj > 127)
            {
                printf("\nneogovarajuci broj, poslat je broj 126\n");
                poruka.broj = 126;
            }
            msgsnd(porukaId, &poruka, sizeof(poruka) - sizeof(long), 0);

        } while (poruka.broj != 0);

        wait(NULL);
        msgctl(porukaId, IPC_RMID, NULL);
    }
    else
    {
        // dete

        int sumaBrojeva = 0;

        do
        {
            msgrcv(porukaId, &poruka, sizeof(poruka) - sizeof(long), 1, 0);
            printf("primljen karakter: %c\n", (char)poruka.broj);
            if (poruka.broj >= 48 && poruka.broj <= 57)
                sumaBrojeva++;

        } while (poruka.broj != 0);

        printf("suma prosledjenih brojeva kao karakter je %d\n", sumaBrojeva);
    }
    return 0;
}