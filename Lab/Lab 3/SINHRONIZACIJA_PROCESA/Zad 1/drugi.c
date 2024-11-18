#include "init.h"

int main(int argc, char * argv[]) {

	int N = atoi(argv[2]);

	srand(time(NULL));

	// Semafori
	int prviId, drugiId;
 	struct sembuf sem_lock = { 0, -1, NULL};
 	struct sembuf sem_unlock = { 0, 1, NULL};

 	// Pribavljanje reference semafora
 	prviId = semget((key_t)PRVI_KEY, 1,  0666);
 	drugiId = semget((key_t)DRUGI_KEY, 1,  0666);


	for (int i = 0; i < N; ++i) {
			
		semop(drugiId, &sem_lock, 1);		// sem_wait()
		
		FILE * f = fopen(argv[1], "a");

		int x, y, z;
		x = rand() % 10 - 10;
		y = rand() % 10 - 10;

		fprintf(f, "%d %d\n", x, y);
		
		fclose(f);

		semop(prviId, &sem_unlock, 1);		// sem_post()
	}
	
	return 0;
}