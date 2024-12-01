#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define MAX_CNTC 5
#define MAX_CNTZ 2

int cntC;
int cntZ;

void klikciC(int sigNum)
{
	signal(SIGINT, klikciC);

	cntC++;

	if (cntC >= MAX_CNTC)
	{
		printf("Izlazim...\n");
		exit(0);
	}
}

void klikciZ(int sigNum)
{
	signal(SIGTSTP, klikciZ);

	cntZ++;

	if (cntZ >= MAX_CNTZ)
	{
		printf("Counter CTRL_C je: %d\n", cntC);
		cntZ = 0;
	}
}

int main(int argc, char* argv[])
{
	signal(SIGINT, klikciC);
	signal(SIGTSTP, klikciZ);
	while (1) pause();

	return 0;
}
