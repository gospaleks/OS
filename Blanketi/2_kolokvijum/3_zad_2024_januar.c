#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_LEN 32
#define KEY 10101

struct student
{
    char ime[MAX_LEN];
    char prezime[MAX_LEN];
    int indeks;
};

struct mymsgbuf
{
    long tip;
    struct student s;
};

void obradi(int msqid, struct mymsgbuf buff, int tip)
{
    int minIndeks = INT_MAX;
    int maxIndeks = INT_MIN;

    while (1)
    {
        msgrcv(msqid, &buff, sizeof(buff) - sizeof(long), tip, 0);

        if (buff.s.indeks == -1)
            break;

        if (buff.s.indeks < minIndeks)
            minIndeks = buff.s.indeks;

        if (buff.s.indeks > maxIndeks)
            maxIndeks = buff.s.indeks;
    }

    printf("A%d: min indeks je %d, max indeks je %d\n", tip, minIndeks, maxIndeks);
}

int main()
{
    int msqid = msgget(KEY, 0666 | IPC_CREAT);
    struct mymsgbuf buff;

    int pid1 = fork();
    if (pid1 == 0)
    {
        // A1
        obradi(msqid, buff, 1);

        return 0;
    }

    int pid2 = fork();
    if (pid2 == 0)
    {
        // A2
        obradi(msqid, buff, 2);

        return 0;
    }

    // roditelj

    FILE *f = fopen("prijava.txt", "r");
    if (f == 0)
        exit(1);

    char linija[3 * MAX_LEN];
    while (fgets(linija, 3 * MAX_LEN, f))
    {
        char *tmp = strtok(linija, " ");
        strcpy(buff.s.ime, tmp);

        tmp = strtok(NULL, " ");
        strcpy(buff.s.prezime, tmp);

        tmp = strtok(NULL, " ");
        buff.s.indeks = atoi(tmp);

        if (buff.s.indeks < 18000)
            buff.tip = 1;
        else
            buff.tip = 2;

        msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);
    }

    fclose(f);

    // Obavesti decu da je kraj (spec vrednost)
    buff.s.indeks = -1;
    buff.tip = 1;
    msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

    buff.tip = 2;
    msgsnd(msqid, &buff, sizeof(buff) - sizeof(long), 0);

    wait(NULL);
    wait(NULL);

    msgctl(msqid, IPC_RMID, NULL);

    return 0;
}