#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 10

#define S1_KEY 10101
#define SHARED_MEM_KEY 10101

union semun {
    int val;
};

int main()
{
    // Kreiranje semafora
    int semid = semget((key_t)S1_KEY, 2, 0666 | IPC_CREAT);

    // Inicijalizacija semafora
    union semun semopts;
    semopts.val = 0;
    semctl(semid, 0, SETVAL, semopts);

    // Definisanje operacija nad semaforima
    struct sembuf sem_wait = {0, -1, 0};
    struct sembuf sem_post = {0, 1, 0};

    // Kreiranje deljene memorije velicine 2 int-a
    int shmemid = shmget(SHARED_MEM_KEY, sizeof(int), IPC_CREAT | 0666);

    for (int i = 0; i < N; ++i)
    {
        int pid = fork();
        if (pid == 0)
        {
            // Mapiranje deljene memorije
            int *shmptr = (int *)shmat(shmemid, NULL, 0);

            // Upisi pid u deljenu memoriju
            *shmptr = getpid();

            semop(semid, &sem_post, 1);

            // Izbacivanje deljene memorije iz adresnog prostora
            shmdt(shmptr);

            return 0;
        }
        else
        {
            // Mapiranje deljene memorije
            int *shmptr = (int *)shmat(shmemid, NULL, 0);

            semop(semid, &sem_wait, 1);
            int pidDeteta = *shmptr;

            printf("dete: %d, roditelj: %d -> ", pidDeteta, getpid());
            if (getpid() + 1 == pidDeteta)
                printf("Razlikuju se za 1.\n");
            else
                printf("Ne razlikuju se za 1.\n");

            wait(NULL);

            // Izbacivanje deljene memorije iz adresnog prostora
            shmdt(shmptr);
        }
    }

    semctl(semid, 0, IPC_RMID, 0); // unisti semafor
    shmctl(shmemid, 0, IPC_RMID);  // unisti shared memory

    return 0;
}