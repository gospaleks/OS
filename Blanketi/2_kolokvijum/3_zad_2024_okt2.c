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

#define MAX_LEN 256
#define KEY 10101

struct mymsgbuf
{
    long tip;
    char linija[MAX_LEN];
};

int main()
{
    struct mymsgbuf buff;
    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    int pid1 = fork();
    if (pid1 == 0)
    {
        buff.tip = 1;

        FILE *f = fopen("prva.txt", "r");
        if (f == 0)
            exit(1);

        while (fgets(buff.linija, MAX_LEN, f))
            msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

        strcpy(buff.linija, "KRAJ");
        msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

        return 0;
    }

    int pid2 = fork();
    if (pid2 == 0)
    {
        buff.tip = 2;

        FILE *f = fopen("druga.txt", "r");
        if (f == 0)
            exit(1);

        while (fgets(buff.linija, MAX_LEN, f))
            msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

        strcpy(buff.linija, "KRAJ");
        msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

        return 0;
    }

    // roditelj

    FILE *f1 = fopen("prva_mod.txt", "w");
    FILE *f2 = fopen("druga_mod.txt", "w");

    int brGotovih = 0;
    while (brGotovih != 2)
    {
        msgrcv(msqid, &buff, sizeof(buff) - sizeof(long), 0, 0); // uzima sve poruke nezavisno od tipa

        if (strcmp(buff.linija, "KRAJ") == 0)
        {
            brGotovih++;
            continue;
        }

        if (buff.tip == 1)
        {
            for (int i = 0; i < strlen(buff.linija); ++i)
                buff.linija[i] = toupper(buff.linija[i]);

            fprintf(f1, "%s", buff.linija);
        }
        else if (buff.tip == 2)
        {
            for (int i = 0; i < strlen(buff.linija); ++i)
                buff.linija[i] = tolower(buff.linija[i]);

            fprintf(f2, "%s", buff.linija);
        }
    }

    fclose(f1);
    fclose(f2);

    wait(NULL);
    wait(NULL);

    msgctl(msqid, IPC_RMID, NULL);

    return 0;
}