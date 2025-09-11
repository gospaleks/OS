#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define KEY 20202
#define MAX_IME 64
#define MAX_PREZIME 64

struct student_msg
{
    long tip;
    int indeks;
    char ime[MAX_IME];
    char prezime[MAX_PREZIME];
};

int main()
{
    int msqid = msgget(KEY, 0666 | IPC_CREAT);

    struct student_msg buf;

    int pid1 = fork();
    if (pid1 == 0)
    {
        // dete 1 - stara akreditacija
        FILE *f = fopen("stara_akreditacija.txt", "w");
        if (!f)
            exit(1);
        int brojac = 0;
        while (1)
        {
            msgrcv(msqid, &buf, sizeof(buf) - sizeof(long), 1, 0);
            if (buf.indeks == 10000)
                break;
            fprintf(f, "%d %s %s\n", buf.indeks, buf.ime, buf.prezime);
            brojac++;
        }
        fclose(f);
        printf("Stara akreditacija: primljeno %d poruka\n", brojac);
        exit(0);
    }

    int pid2 = fork();
    if (pid2 == 0)
    {
        // dete 2 - nova akreditacija
        FILE *f = fopen("nova_akreditacija.txt", "w");
        if (!f)
            exit(1);
        int brojac = 0;
        while (1)
        {
            msgrcv(msqid, &buf, sizeof(buf) - sizeof(long), 2, 0);
            if (buf.indeks == 10000)
                break;
            fprintf(f, "%d %s %s\n", buf.indeks, buf.ime, buf.prezime);
            brojac++;
        }
        fclose(f);
        printf("Nova akreditacija: primljeno %d poruka\n", brojac);
        exit(0);
    }

    // roditelj
    while (1)
    {
        printf("Unesite broj indeksa: ");
        scanf("%d", &buf.indeks);
        if (buf.indeks == 10000)
        {
            // šaljemo kraj oba deteta
            buf.tip = 1;
            msgsnd(msqid, &buf, sizeof(buf) - sizeof(long), 0);
            buf.tip = 2;
            msgsnd(msqid, &buf, sizeof(buf) - sizeof(long), 0);
            break;
        }
        printf("Unesite ime: ");
        scanf("%s", buf.ime);
        printf("Unesite prezime: ");
        scanf("%s", buf.prezime);

        if (buf.indeks < 17000)
            buf.tip = 1;
        else
            buf.tip = 2;

        msgsnd(msqid, &buf, sizeof(buf) - sizeof(long), 0);
    }

    wait(NULL);
    wait(NULL);

    msgctl(msqid, IPC_RMID, NULL);

    return 0;
}