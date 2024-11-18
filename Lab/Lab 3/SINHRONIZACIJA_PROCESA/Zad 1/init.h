#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

// U principu ovo key moze bude bilo sta (bitno da svi procesi znaju)

#define PRVI_KEY 10101
#define DRUGI_KEY 10102