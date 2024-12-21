#include <ctype.h>
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
    long type;
    char linija[MAX_LEN];
    int kraj; // flag da se oznaci kad je kraj
};

// Moze f-ja jer procesi rade maltene istu stvar samo se jedan prebacuje u mala drugi u velika
void konvertuj(int msqid, int type)
{
    struct mymsgbuf buff;
    char konvertovano[MAX_LEN];
    while (1)
    {
        msgrcv(msqid, &buff, sizeof(buff) - sizeof(long), type, 0); // ovde type (svako dete ima svoj koji ocekuje)

        if (buff.kraj == 1)
            break;

        int i = 0;
        for (; i < strlen(buff.linija); ++i)
            konvertovano[i] = type == 1 ? tolower(buff.linija[i]) : toupper(buff.linija[i]);

        konvertovano[i] = '\0';

        FILE *f = fopen("sredjeno.txt", "a");
        if (f == 0)
            exit(1);
        fprintf(f, "%s", konvertovano);
        fclose(f);
    }
}

int main()
{
    // override prethodno stanje jer procesi rade append
    FILE *ff = fopen("sredjeno.txt", "w");
    if (ff == 0)
        exit(1);
    fclose(ff);

    // Kreiranje reda poruka
    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    // Deca
    int pid1 = fork();
    if (pid1 == 0)
    {
        konvertuj(msqid, 1);
        return 0;
    }

    int pid2 = fork();
    if (pid2 == 0)
    {
        konvertuj(msqid, 2);
        return 0;
    }

    // Roditelj

    struct mymsgbuf buff;
    buff.kraj = 0;

    FILE *f = fopen("poruka.txt", "r");
    if (f == 0)
        exit(1);

    int brReda = 0;
    char linija[MAX_LEN];
    while (fgets(linija, MAX_LEN, f))
    {
        if (brReda)
            buff.type = 2; // parni redovi
        else
            buff.type = 1; // neparni redovi

        strcpy(buff.linija, linija);
        msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

        brReda ^= 1;
    }

    fclose(f);

    // Posalji deci poruku da je kraj tako sto se set-uje flag za kraj
    buff.kraj = 1;
    buff.type = 1;
    msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);
    buff.type = 2;
    msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

    // Cekaj da deca zavrse
    wait(NULL);
    wait(NULL);

    // Unisti red poruka
    msgctl(msqid, IPC_RMID, NULL);

    return 0;
}