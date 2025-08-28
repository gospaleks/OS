#include <stdio.h>
#include <stdlib.h>
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

#define N 10

union semun {
    int val;
};

int main()
{
    // Kreiranje semafora
    int sem1id = semget((key_t)S1_KEY, 1, 0666 | IPC_CREAT);
    int sem2id = semget((key_t)S2_KEY, 1, 0666 | IPC_CREAT);

    // Inicijalizacija semafora
    union semun semopts;
    semopts.val = 1;
    semctl(sem1id, 0, SETVAL, semopts);
    semopts.val = 0;
    semctl(sem2id, 0, SETVAL, semopts);

    // Definisanje operacija nad semaforima
    struct sembuf sem_wait = {0, -1, 0};
    struct sembuf sem_post = {0, 1, 0};

    // Kreiranje deljene memorije velicine N+1 int-a jer poslednji koristimo kao flag za kraj
    int shmemid = shmget(SHARED_MEM_KEY, (N + 1) * sizeof(int), IPC_CREAT | 0666);

    int pid = fork();
    if (pid != 0)
    {
        // Mapiranje deljene memorije
        int *shmptr = (int *)shmat(shmemid, NULL, 0);

        FILE *f = fopen("brojevi.txt", "r");
        if (f == 0)
            exit(1);

        while (1)
        {
            semop(sem1id, &sem_wait, 1);

            int i = 0;
            while (i < N && fscanf(f, "%d", &shmptr[i]) == 1)
                ++i;

            if (i < N)
            {
                while (i < N)
                    shmptr[i++] = 0;

                shmptr[N] = -1;

                semop(sem2id, &sem_post, 1);

                break;
            }

            semop(sem2id, &sem_post, 1);
        }

        fclose(f);

        wait(NULL);

        // Izbacivanje deljene memorije iz adresnog prostora
        shmdt(shmptr);
    }
    else
    {
        // Mapiranje deljene memorije
        int *shmptr = (int *)shmat(shmemid, NULL, 0);

        while (1)
        {
            semop(sem2id, &sem_wait, 1);

            int suma = 0;
            for (int i = 0; i < N; ++i)
            {
                printf("%3d ", shmptr[i]);
                suma += shmptr[i];
            }
            printf(" -> suma: %d\n", suma);

            if (shmptr[N] == -1)
                break;

            semop(sem1id, &sem_post, 1);
        }

        // Izbacivanje deljene memorije iz adresnog prostora
        shmdt(shmptr);

        return 0;
    }

    semctl(sem1id, 0, IPC_RMID, 0); // unisti semafor 1
    semctl(sem2id, 0, IPC_RMID, 0); // unisti semafor 2
    shmctl(shmemid, 0, IPC_RMID);   // unisti shared memory
}