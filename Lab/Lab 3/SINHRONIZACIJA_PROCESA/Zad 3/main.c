#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#define PRVI_KEY 10101
#define DRUGI_KEY 10102

union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};

int main(int argc, char * argv[]) {

   srand(time(NULL));

   // da obrise prethodni sadrzaj
   FILE * f = fopen("brojevi.dat", "w");
   if (f == 0) return 1;
   fclose(f);

   // Kreiranje semafora
   int prviId, drugiId;
   prviId = semget((key_t)PRVI_KEY, 1, 0666 | IPC_CREAT);
   drugiId = semget((key_t)DRUGI_KEY, 1, 0666 | IPC_CREAT);

   // Inicijalizacija semafora
   union semun semopts;
   semopts.val = 1;
   semctl(prviId, 0, SETVAL, semopts);
   semopts.val = 0;
   semctl(drugiId, 0, SETVAL, semopts);

   struct sembuf sem_lock = { 0, -1, 0};
   struct sembuf sem_unlock = { 0, 1, 0};

   if (fork() == 0) {
      // proces 1
      int niz[3] = {0};
      for (int i = 0; i < 20; ++i) {        
         semop(prviId, &sem_lock, 1);        // sem_wait()
         
         FILE * f = fopen("brojevi.dat", "a");
         
         for (int j = 0; j < 3; ++j) {
            niz[j] = rand() % 10 + 1;
            fprintf(f, "%d ", niz[j]);
         }
         fprintf(f, "\n");

         fclose(f);

         semop(drugiId, &sem_unlock, 1);      // sem_post()
      }

      return 0;
   }

   if (fork() == 0) {
      // proces 2
      for (int i = 0; i < 20; ++i) {        
         semop(drugiId, &sem_lock, 1);       // sem_wait()
         
         FILE * f = fopen("brojevi.dat", "a");
         int br = rand() % 10 - 11;
         fprintf(f, "%d\n", br);
         fclose(f);

         semop(prviId, &sem_unlock, 1);      // sem_post()
      }

      return 0;
   }

   wait(NULL);
   wait(NULL);

   return 0;
} 