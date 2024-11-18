#include "init.h"

int main(int argc, char * argv[]) {

	srand(time(NULL));

	int N = atoi(argv[2]);

	// Semafori
	int prviId, drugiId;
 	struct sembuf sem_lock = { 0, -1, NULL};
 	struct sembuf sem_unlock = { 0, 1, NULL};

 	// Pribavljanje reference semafora
 	prviId = semget((key_t)PRVI_KEY, 1,  0666);
 	drugiId = semget((key_t)DRUGI_KEY, 1,  0666);

	for (int i = 0; i < N; ++i) {
			
		semop(prviId, &sem_lock, 1);		// sem_wait()
		
		FILE * f = fopen(argv[1], "a");

		int x, y, z;
		x = rand() % 10 + 1;
		y = rand() % 10 + 1;
		z = rand() % 10 + 1;

		fprintf(f, "%d %d %d \n", x, y, z);

		fclose(f);

		semop(drugiId, &sem_unlock, 1);		// sem_post()
	}

	return 0;
}