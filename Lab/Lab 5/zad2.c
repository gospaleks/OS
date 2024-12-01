#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define MSG_KEY 12002

struct mymsgbuf
{
	long tip;
	int broj;
};

int main(int argc, char ** argv)
{
	int msqid;
	struct mymsgbuf buf;
	buf.tip = 3;

	msqid = msgget(MSG_KEY, 0666 | IPC_CREAT);

	int pid = fork();
	if (pid != 0)
	{
		// roditelj
		do
		{
			printf("Unesite broj: ");
			scanf("%d", &buf.broj);

			if (buf.broj > 0)
				msgsnd(msqid, &buf, sizeof(buf) - sizeof(long), 0);

		} while (buf.broj != 787);

		wait(NULL);	
		msgctl(msqid, IPC_RMID, NULL);
	}
	else
	{
		// dete
		FILE * f = fopen("izlaz.txt", "w");

		while (1)
		{
			msgrcv(msqid, &buf, sizeof(buf) - sizeof(long), 3, 0);
			if (buf.broj == 787)
				break;
			fprintf(f, "%d\n", buf.broj);
		}

		fclose(f);
	}
}