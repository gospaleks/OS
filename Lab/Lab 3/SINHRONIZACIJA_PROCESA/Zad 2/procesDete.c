#include "init.h"

int main(int argc, char * argv[]) {

	srand(time(NULL));

	// Semafori
	int prviId, drugiId;
 	struct sembuf sem_lock = { 0, -1, NULL};
 	struct sembuf sem_unlock = { 0, 1, NULL};

 	// Pribavljanje reference semafora
 	prviId = semget((key_t)PRVI_KEY, 1,  0666);
 	drugiId = semget((key_t)DRUGI_KEY, 1,  0666);

	for (int i = 2; i <= 20; i += 2) {
			
		semop(drugiId, &sem_lock, 1);		// sem_wait()
		
		FILE * f = fopen(argv[1], "a");

		fprintf(f, "%d\n", i);

		fclose(f);

		semop(prviId, &sem_unlock, 1);		// sem_post()
	}

	return 0;
}