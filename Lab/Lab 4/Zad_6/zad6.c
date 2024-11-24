#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define N 25

int main()
{
    srand(time(NULL));

    int pd[2];
    if (pipe(pd) < 0)
        return 1;

    int pid = fork();

    if (pid == 0)
    {
        FILE *f = fopen("chars.txt", "w");
        close(pd[1]);
        char c;
        while (read(pd[0], &c, sizeof(char)) > 0)
            fprintf(f, "%c", c);
        fprintf(f, "\n");
        fclose(f);
        close(pd[0]);
    }
    else
    {
        close(pd[0]);
        for (int i = 0; i < N; i++)
        {
            char c = 'a' + rand() % 26;
            write(pd[1], &c, sizeof(char));
        }
        close(pd[1]);
        wait(NULL);
    }
    return 0;
}
