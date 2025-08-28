#include "init.h"

int main()
{
    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    int pid1 = fork();
    if (pid1 == 0)
    {
        if (execl("./prvi", "./prvi", NULL) < 0)
            exit(1);

        return 0;
    }

    int pid2 = fork();
    if (pid2 == 0)
    {
        if (execl("./drugi", "./drugi", NULL) < 0)
            exit(1);

        return 0;
    }

    wait(NULL);
    wait(NULL);

    msgctl(msqid, IPC_RMID, NULL);

    return 0;
}