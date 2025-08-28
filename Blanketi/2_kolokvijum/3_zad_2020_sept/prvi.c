#include "init.h"

int main()
{
    printf("~ 1. proces pokrenut!\n");
    struct BaferPoruke bafer;
    bafer.tip = 1;
    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    do
    {
        scanf(" %[^\n]", bafer.linija);

        msgsnd(msqid, &bafer, sizeof(bafer) - sizeof(long), 0);

    } while(strcmp(bafer.linija, "KRAJ"));

    printf("~ Prvi proces zavrsio slanje\n");

    return 0;
}