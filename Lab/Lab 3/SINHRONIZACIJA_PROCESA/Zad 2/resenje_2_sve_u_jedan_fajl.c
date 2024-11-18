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

/*
	ovo je drugi nacin da se ne poziva execl i da imamo poseban fajl za proces dete
	samo u posle forka celo telo deteta napisemo u if delu, a u else ostaje roditelj
	mozda je jednostavnije za pracenje
*/

union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};

int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("Neodgovarajuci broj argumenata.\n");
		return 1;
	}

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
	
	// Operacije
	struct sembuf sem_lock = { 0, -1, NULL};
	struct sembuf sem_unlock = { 0, 1, NULL};

	int pid = fork();
	if (pid == 0) {
		// Ovo izvrsava dete
		for (int i = 2; i <= 20; i += 2) {			
			semop(drugiId, &sem_lock, 1);		// sem_wait()
			
			FILE * f = fopen(argv[1], "a");
			fprintf(f, "%d\n", i);
			fclose(f);

			semop(prviId, &sem_unlock, 1);		// sem_post()
		}
	}
	else {
		// Ovo izvrsava roditelj
	 	for (int i = 1; i < 20; i += 2) {
	 		
	 		semop(prviId, &sem_lock, 1);

			FILE * f = fopen(argv[1], "a");
			fprintf(f, "%d\n", i);
			fclose(f);
			
	 		semop(drugiId, &sem_unlock, 1);
	 	}

	 	wait(NULL);
 	}

 	return 0;
}