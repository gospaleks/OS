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

union semun {
    int val;
};

struct data
{
    int niz[10];
    int flag;
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
    int shmemid = shmget(SHARED_MEM_KEY, sizeof(struct data), IPC_CREAT | 0666);

    int pid = fork();
    if (pid != 0)
    {
        // Mapiranje deljene memorije
        struct data *shmptr = (struct data *)shmat(shmemid, NULL, 0);

        FILE *f = fopen("brojevi.txt", "r");
        if (f == 0)
            exit(1);

        shmptr->flag = 0;
        while (shmptr->flag != 1)
        {
            semop(sem1id, &sem_wait, 1);

            int brProcitanih = 0;

            while (brProcitanih < 10)
            {
                if (fscanf(f, "%d", &shmptr->niz[brProcitanih]) == EOF)
                {
                    // Oznaci da je kraj
                    shmptr->flag = 1;

                    // Dopuni nulama
                    if (brProcitanih != 0)
                        while (brProcitanih < 10)
                            shmptr->niz[brProcitanih++] = 0;

                    break;
                }

                ++brProcitanih;
            }

            semop(sem2id, &sem_post, 1);
        }

        fclose(f);

        wait(NULL);

        // Izbacivanje deljene memorije iz adresnog prostora
        shmdt(shmptr);

        semctl(sem1id, 0, IPC_RMID, 0); // unisti semafor 1
        semctl(sem2id, 0, IPC_RMID, 0); // unisti semafor 2
        shmctl(shmemid, 0, IPC_RMID);   // unisti shared memory
    }
    else
    {
        // Mapiranje deljene memorije
        struct data *shmptr = (struct data *)shmat(shmemid, NULL, 0);

        shmptr->flag = 0;
        while (shmptr->flag != 1)
        {
            semop(sem2id, &sem_wait, 1);

            // Ako je kraj i zadnji nije 0 znaci da nije bilo dopune i to je vec ispisano
            // ako bi se i tad ispisalo imali bi ponovljen niz
            if (shmptr->flag == 1 && shmptr->niz[9] != 0)
                break;

            for (int i = 0; i < 10; ++i)
                printf("%d ", shmptr->niz[i]);
            printf("\n");

            semop(sem1id, &sem_post, 1);
        }

        // Izbacivanje deljene memorije iz adresnog prostora
        shmdt(shmptr);

        return 0;
    }
}