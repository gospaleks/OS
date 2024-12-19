#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define S1_KLJUC 10101
#define S2_KLJUC 10102
#define DELJENA_MEM_KLJUC 10103

union semun
{
    int vrednost;
};

int main()
{
    // kreiranje semafora
    int sem1id = semget((key_t)S1_KLJUC, 2, 0666 | IPC_CREAT);
    int sem2id = semget((key_t)S2_KLJUC, 2, 0666 | IPC_CREAT);

    // inicijalizacija semafora
    union semun semopts;
    semopts.vrednost = 0;
    semctl(sem1id, 0, SETVAL, semopts);
    semctl(sem2id, 0, SETVAL, semopts);

    // definisanje operacija nad semaforima
    struct sembuf sem_wait = {0, -1, 0};
    struct sembuf sem_post = {0, 1, 0};

    // kreiranje deljene memorije velicine 5 * int
    int deljenaMemId = shmget(DELJENA_MEM_KLJUC, 5 * sizeof(int), IPC_CREAT | 0666);

    int pid = fork();
    if (pid != 0)
    {
        // roditelj

        // mapiranje deljene memorije
        int *broj = (int *)shmat(deljenaMemId, NULL, 0);

        while (1)
        {
            for (int i = 0; i < 5; i++)
                scanf("%d", &broj[i]);

            semop(sem2id, &sem_post, 1);

            semop(sem1id, &sem_wait, 1);

            printf("suma = %d\n", broj[0]);

            if (broj[0] == 0)
                break;
        }

        // izbacivanje deljene memorije iz adresnog prostora
        shmdt(broj);
        // ceka se dete
        wait(NULL);
        // brisanje semafora
        semctl(sem1id, 0, IPC_RMID, 0);
        semctl(sem2id, 0, IPC_RMID, 0);
        // brisanje deljene memorije
        shmctl(deljenaMemId, 0, IPC_RMID);
    }
    else
    {
        // dete

        // mapiranje deljene memorije
        int *broj = (int *)shmat(deljenaMemId, NULL, 0);

        while (1)
        {
            semop(sem2id, &sem_wait, 1);

            int suma = 0;

            for (int i = 0; i < 5; i++)
                suma += broj[i];

            broj[0] = suma;

            semop(sem1id, &sem_post, 1);

            if (suma == 0)
                break;
        }
        // izbacivanje deljene memorije iz adresnog prostora
        shmdt(broj);
    }
    return 0;
}