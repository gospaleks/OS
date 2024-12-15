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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Neodgovarajuci broj argumenata.\n");
        exit(1);
    }

    struct mymsgbuf buf;
    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    int pid1 = fork();
    if (pid1 == 0)
    {
        // dete 1
        FILE *f = fopen("prva.txt", "w");

        while (1)
        {
            msgrcv(msqid, &buf, sizeof(buf) - sizeof(long), 1, 0);

            if (strcmp(buf.line, "KRAJ") == 0)
                break;

            fprintf(f, "%s", buf.line);
        }

        fclose(f);

        return 0;
    }

    int pid2 = fork();
    if (pid2 == 0)
    {
        // dete 2
        FILE *f = fopen("druga.txt", "w");

        while (1)
        {
            msgrcv(msqid, &buf, sizeof(buf) - sizeof(long), 2, 0);

            if (strcmp(buf.line, "KRAJ") == 0)
                break;

            fprintf(f, "%s", buf.line);
        }

        fclose(f);

        return 0;
    }

    // roditelj
    FILE *f = fopen(argv[1], "r");
    if (f == 0)
        exit(1);

    while (fgets(buf.line, MAX_LEN, f))
    {
        if (buf.line[0] == 'Z' || buf.line[0] == 'z')
            buf.tip = 1;
        else
            buf.tip = 2;

        msgsnd(msqid, &buf, sizeof(buf) - sizeof(long), 0);
    }

    fclose(f);

    strcpy(buf.line, "KRAJ");

    buf.tip = 1;
    msgsnd(msqid, &buf, sizeof(buf) - sizeof(long), 0);
    buf.tip = 2;
    msgsnd(msqid, &buf, sizeof(buf) - sizeof(long), 0);

    wait(NULL);
    wait(NULL);

    msgctl(msqid, IPC_RMID, NULL);

    return 0;
}
