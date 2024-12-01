#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void catch_alarm(int sig_num)
{
	printf("Operacija je istekla. Izlazim...\n\n");
	exit(0);
}

int main(int argc, char* argv[])
{
	char username[40];
	char password[40];

	signal(SIGALRM, catch_alarm);

	printf("Username: ");
	fflush(stdout);
	gets(username);

	alarm(6);
		
	printf("Password: ");
	fflush(stdout);
	gets(password);

	alarm(0);

	printf("Username: '%s'\n", username);
	printf("Password: '%s'\n", password);


	return 0;
}
