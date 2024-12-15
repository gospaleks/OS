#include "init.h"

int main()
{
    struct mymsgbuf buff;
    buff.tip = 1;
    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    int suma = 0;

    do
    {
        scanf("%d", &buff.broj);
        suma += buff.broj;
        msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

    } while (suma < 1000);

    printf("prvi proces zavrsio slanje...\n");

    return 0;
}