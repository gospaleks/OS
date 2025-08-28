#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

/*
    postoji greska u tekstu...
    tekst zadatka: Koriccenjem prog. jezika C napisati Unix/Linux program koji se deli
    na 2 procesa. Proces roditelj sa standardnog ulaza cita liniju teksta i koriscenjem
    deljene memorije procitane linije teksta prosledjuje procesu detetu.
    Proces dete stampa na standardnom izlazu linije teksta koje prihvata iz deljene memorija.
    Obezbediti da proces dete stampa linije teksta onim redosledom kako se javljaju u tekstualnoj dadoteci.
    Ovaj proces se pponavlja sve dok korisnik na standardnom ulazu ne unese rec "KRAJ".

    tako da radimo 2 verzije:
    verzija 1 (ovaj kod) - citanje sa standardnog ulaza dok se ne unese KRAJ
    verzija 2 - citanje iz fajla dok se ne naidje na liniju KRAJ
*/

#define S1_KLJUC 10101
#define S2_KLJUC 10102
#define DELJENA_MEM_KLJUC 10103
#define MEM_VELICINA 64

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
    // Inicijalizacija semafora
    union semun semopts;
    semopts.vrednost = 1;
    semctl(sem1id, 0, SETVAL, semopts);
    semopts.vrednost = 0;
    semctl(sem2id, 0, SETVAL, semopts);

    struct sembuf sem_wait = {0, -1, 0};
    struct sembuf sem_post = {0, 1, 0};

    // kreiranje deljene memorije velicine 64 char
    int deljenaMemId = shmget(DELJENA_MEM_KLJUC, MEM_VELICINA * sizeof(char), IPC_CREAT | 0666);

    int pid = fork();
    if (pid != 0)
    {
        // roditelj

        // mapiranje deljene memorije
        char *linija = (char *)shmat(deljenaMemId, NULL, 0);

        do
        {
            semop(sem1id, &sem_wait, 1);

            printf("proces RODITELJ - unesite liniju: ");
            fgets(linija, MEM_VELICINA, stdin);

            semop(sem2id, &sem_post, 1);

        } while (strcmp(linija, "KRAJ\n") != 0);

        shmdt(linija);

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

        char *linija = (char *)shmat(deljenaMemId, NULL, 0);

        do
        {
            semop(sem2id, &sem_wait, 1);

            printf("proces DETE - procitana linija: %s", linija);

            semop(sem1id, &sem_post, 1);

        } while (strcmp(linija, "KRAJ\n") != 0);

        shmdt(linija);
    }
    return 0;
}