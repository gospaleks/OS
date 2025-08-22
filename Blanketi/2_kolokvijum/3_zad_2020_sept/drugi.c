#include "init.h"

int main() 
{
    printf("# 2. proces pokrenut!\n");
    struct BaferPoruke bafer;
    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    do 
    {
        msgrcv(msqid, &bafer, sizeof(bafer) - sizeof(long), 1, 0);
        printf("# 2. proc: %s\n", bafer.linija);

    } while(strcmp(bafer.linija, "KRAJ"));

    printf("# Drugi proces zavrsio stampanje\n");

    return 0;
}