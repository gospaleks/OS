#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define MSG_KEY 10101

struct mymsgbuf
{
	long tip;
	int broj;
};

int main(int argc, char ** argv)
{
	srand(time(NULL));

	struct mymsgbuf buf;
	buf.tip = 1;

	int msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);

	int pid = fork();
	if (pid == 0)
	{
		do
		{
			msgrcv(msgid, &buf, sizeof(buf) - sizeof(long), 1, 0);
			printf("Primio sam %d\n", buf.broj);
			fflush(stdout);
		} while (buf.broj != -1);

		printf("Zavrsavam...\n");
	}
	else
	{
		int n = rand() % 20 + 1;

		for (int i = 0; i < n; ++i)
		{
			buf.broj = rand() % 100 + 1;
			printf("Generisan: %d i saljem...\n", buf.broj);
			fflush(stdout);
			msgsnd(msgid, &buf, sizeof(buf) - sizeof(long), 0);
		}

		printf("Saljem -1 za kraj...\n");
		buf.broj = -1;
		msgsnd(msgid, &buf, sizeof(buf) - sizeof(long), 0);
	
		wait(NULL);
		msgctl(msgid, IPC_RMID, NULL);
	}

	return 0;
}