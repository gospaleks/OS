#include "init.h"

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

	// ovo je samo da obrise prethodni sadrzaj fajla
	FILE * f = fopen(argv[1], "w");
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

	if (fork() == 0) {
		execl("./procesDete", "./procesDete", argv[1], NULL);
	}

	// Ovo izvrsava roditelj
	struct sembuf sem_lock = { 0, -1, NULL};
 	struct sembuf sem_unlock = { 0, 1, NULL};

 	for (int i = 1; i < 20; i += 2) {
 		
 		semop(prviId, &sem_lock, 1);

 		FILE * f = fopen(argv[1], "a");

 		fprintf(f, "%d\n", i);

 		fclose(f);

 		semop(drugiId, &sem_unlock, 1);
 	}

 	wait(NULL);

 	return 0;
}