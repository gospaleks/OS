#include "init.h"

int main()
{
    struct mymsgbuf buff;
    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    int suma = 0;

    do
    {
        msgrcv(msqid, &buff, sizeof(buff) - sizeof(long), 1, 0);
        suma += buff.broj;
        printf("primljen broj: %d, suma: %d\n", buff.broj, suma);

    } while (suma < 1000);

    printf("drugi proces zavrsio primanje...\n");

    return 0;
}