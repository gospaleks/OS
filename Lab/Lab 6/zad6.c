#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SHARED_MEM_KEY 10101
#define SEM_KEY 10102
#define MAX_LEN 256

struct data
{
    char linija[MAX_LEN];
    int flag;
};

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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("neodgovarajuci broj argumenata!\n");
        exit(1);
    }

    int semID = semget((key_t)SEM_KEY, 2, 0666 | IPC_CREAT);

    union semun semopts;
    semopts.val = 1;
    semctl(semID, 0, SETVAL, semopts); // s1
    semopts.val = 0;
    semctl(semID, 1, SETVAL, semopts); // s2

    int shmemid = shmget(SHARED_MEM_KEY, sizeof(struct data), IPC_CREAT | 0666);

    int pid = fork();

    if (pid != 0)
    {
        // Mapiranje deljene memorije
        struct data *shmptr = (struct data *)shmat(shmemid, NULL, 0);

        // roditelj
        FILE *f = fopen(argv[1], "r");
        if (f == NULL)
            exit(1);

        while (1)
        {

            semV_wait(semID, 0);
            if (fgets(shmptr->linija, MAX_LEN, f) == NULL)
            {
                shmptr->flag = 1;
                break;
            }
            semV_post(semID, 1);
        }

        semV_post(semID, 1);

        // Izbacivanje deljene memorije iz adresnog prostora
        shmdt(shmptr);

        wait(NULL);

        semctl(semID, 0, IPC_RMID, 0);
        semctl(semID, 1, IPC_RMID, 0);
        shmctl(shmemid, 0, IPC_RMID);
    }
    else
    {
        // dete
        // Mapiranje deljene memorije
        struct data *shmptr = (struct data *)shmat(shmemid, NULL, 0);

        while (1)
        {
            semV_wait(semID, 1);
            if (shmptr->flag)
                break;
            printf("%s", shmptr->linija);
            semV_post(semID, 0);
        }

        // Izbacivanje deljene memorije iz adresnog prostora
        shmdt(shmptr);
    }

    return 0;
}