#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define S1_KEY 10101
#define S2_KEY 10102
#define SHARED_MEM_KEY 10101

union semun {
    int val;
};

int main()
{
    // Kreiranje semafora
    int sem1id = semget((key_t)S1_KEY, 2, 0666 | IPC_CREAT);
    int sem2id = semget((key_t)S2_KEY, 2, 0666 | IPC_CREAT);

    // Inicijalizacija semafora
    union semun semopts;
    semopts.val = 1;
    semctl(sem1id, 0, SETVAL, semopts);
    semopts.val = 0;
    semctl(sem2id, 0, SETVAL, semopts);

    // Definisanje operacija nad semaforima
    struct sembuf sem_wait = {0, -1, 0};
    struct sembuf sem_post = {0, 1, 0};

    // Kreiranje deljene memorije velicine 2 int-a
    int shmemid = shmget(SHARED_MEM_KEY, 2 * sizeof(int), IPC_CREAT | 0666);

    int pid = fork();
    if (pid != 0)
    {
        // Mapiranje deljene memorije
        int *shmptr = (int *)shmat(shmemid, NULL, 0);

        char linija[10];
        while (1)
        {
            semop(sem1id, &sem_wait, 1);

            printf("Unesi broj: ");
            scanf("%s", linija);

            if (strcmp(linija, "KRAJ") == 0)
            {
                shmptr[0] = -1;
                shmptr[1] = -1;
                break;
            }

            int broj = atoi(linija);
            shmptr[0] = broj;
            shmptr[1] = broj * 3;

            semop(sem2id, &sem_post, 1);
        }

        semop(sem2id, &sem_post, 1);

        // Izbacivanje deljene memorije iz adresnog prostora
        shmdt(shmptr);

        // Ceka dete
        wait(NULL);

        // Unisti semafore i deljenu memoriju
        semctl(sem1id, 0, IPC_RMID, 0);
        semctl(sem2id, 0, IPC_RMID, 0);
        shmctl(shmemid, 0, IPC_RMID);
    }
    else
    {
        // Mapiranje deljene memorije
        int *shmptr = (int *)shmat(shmemid, NULL, 0);

        while (1)
        {
            semop(sem2id, &sem_wait, 1);

            if (shmptr[0] == -1 && shmptr[1] == -1)
                break;

            printf("%d %d, zbir: %d\n", shmptr[0], shmptr[1], shmptr[0] + shmptr[1]);

            semop(sem1id, &sem_post, 1);
        }

        // Izbacivanje deljene memorije iz adresnog prostora
        shmdt(shmptr);
    }

    return 0;
}