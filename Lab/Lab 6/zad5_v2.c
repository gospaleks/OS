#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LEN 256

#define SEM_KEY 10101
#define SHARED_MEM_KEY 10104

union semun {
    int val;
};

void semV_wait(int semid, int semnum)
{
    struct sembuf sop = {semnum, -1, 0};
    semop(semid, &sop, 1);
}

void semV_post(int semid, int semnum)
{
    struct sembuf sop = {semnum, 1, 0};
    semop(semid, &sop, 1);
}

void citaj(int semid, int semnum, int shmemid, char *imeFajla)
{
    // Mapiranje deljene memorije
    char *shmptr = (char *)shmat(shmemid, NULL, 0);

    FILE *f = fopen(imeFajla, "r");
    if (f == 0)
        exit(1);

    while (1)
    {
        semV_wait(semid, semnum);
        if (fgets(shmptr, MAX_LEN, f) == NULL)
            break;
        semV_post(semid, 2);
    }

    semV_post(semid, 3); // Signaliziraj da je dete zavrsilo
    semV_post(semid, 2);

    fclose(f);

    // Izbacivanje deljene memorije iz adresnog prostora
    shmdt(shmptr);
}

// U realizcaji se podrazumeva da ulazni fajlovi imaju isti broj linija!
int main()
{
    // Kreiranje semafora
    int semId = semget((key_t)SEM_KEY, 4, 0666 | IPC_CREAT);

    // Inicijalizacija semafora
    union semun semopts;
    semopts.val = 1;
    semctl(semId, 0, SETVAL, semopts); // S1
    semopts.val = 0;
    semctl(semId, 1, SETVAL, semopts); // S2
    semctl(semId, 2, SETVAL, semopts); // S3
    semctl(semId, 3, SETVAL, semopts); // S4

    // Kreiranje deljene memorije
    int shmemid = shmget(SHARED_MEM_KEY, MAX_LEN * sizeof(char), IPC_CREAT | 0666);

    int pid1 = fork();
    if (pid1 == 0)
    {
        citaj(semId, 0, shmemid, "ulaz1.txt");
        return 0;
    }

    int pid2 = fork();
    if (pid2 == 0)
    {
        citaj(semId, 1, shmemid, "ulaz2.txt");
        return 0;
    }

    // Roditelj
    // Mapiranje deljene memorije
    char *shmptr = (char *)shmat(shmemid, NULL, 0);

    FILE *f = fopen("izlaz.txt", "w");

    int flag = 0, i = 1;
    while (1)
    {
        semV_wait(semId, 2);

        // Check the value of semaphore 4
        int sem4_val = semctl(semId, 3, GETVAL, 0);
        if (sem4_val == 1)
            break;

        fprintf(f, "%d: %s", i++, shmptr);
        if (flag)
        {
            flag = 0;
            semV_post(semId, 0);
        }
        else
        {
            flag = 1;
            semV_post(semId, 1);
        }
    }

    if (flag)
        semV_post(semId, 0);
    else
        semV_post(semId, 1);

    fclose(f);

    // Izbacivanje deljene memorije iz adresnog prostora
    shmdt(shmptr);

    wait(NULL);
    wait(NULL);

    semctl(semId, 0, IPC_RMID, 0);
    semctl(semId, 1, IPC_RMID, 0);
    semctl(semId, 2, IPC_RMID, 0);
    semctl(semId, 3, IPC_RMID, 0);
    shmctl(shmemid, 0, IPC_RMID);

    return 0;
}
