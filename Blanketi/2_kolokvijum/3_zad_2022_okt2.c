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
};

int main()
{
    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    struct mymsgbuf buff;

    int pid1 = fork();
    if (pid1 == 0)
    {
        // dete 1
        while (1)
        {
            msgrcv(msqid, &buff, sizeof(buff) - sizeof(long), 1, 0);

            if (strcmp(buff.line, "KRAJ") == 0)
                break;

            buff.line[strlen(buff.line) - 1] = '\0';
            printf("%s, %ld\n", buff.line, strlen(buff.line));
        }

        return 0;
    }

    int pid2 = fork();
    if (pid2 == 0)
    {
        // dete 2
        while (1)
        {
            msgrcv(msqid, &buff, sizeof(buff) - sizeof(long), 2, 0);

            if (strcmp(buff.line, "KRAJ") == 0)
                break;

            int len = strlen(buff.line);
            for (int i = 0; i < len; ++i)
                printf("%c", toupper(buff.line[i]));
        }

        return 0;
    }

    // roditelj
    while (1)
    {
        scanf("%ld", &buff.tip);
        getchar();

        if (buff.tip != 1 && buff.tip != 2)
            break;

        fgets(buff.line, MAX_LEN, stdin);

        msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);
    }

    // posalji na oba string KRAJ da bi prekinuli
    strcpy(buff.line, "KRAJ");

    buff.tip = 1;
    msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

    buff.tip = 2;
    msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

    wait(NULL);
    wait(NULL);

    msgctl(msqid, IPC_RMID, NULL);

    return 0;
}