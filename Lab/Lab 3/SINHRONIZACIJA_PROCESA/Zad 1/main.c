#include "init.h"

// Proces roditelj mora da ima ovo (tako pise u manual, i ne radi onako jbg...)
// u 'man semctl' pise to

union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};

int main(int argc, char * argv[]) {

	if (argc != 3) {
		printf("Neodgovarajuci broj argumenata.\n");
		return 1;
	}

	// Otvaramo fajl i zatvaramo samo da obrisemo prethodni sadrzaj
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

	// Pokretanje procesa prvi
	if (fork() == 0) {
		execl("./prvi", "./prvi", argv[1], argv[2], NULL);
	}

	if (fork() == 0) {
		execl("./drugi", "./drugi", argv[1], argv[2], NULL);
	}

	wait(NULL);
	wait(NULL);

	return 0;
}