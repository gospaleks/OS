#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>

pthread_mutex_t lock;
pthread_cond_t cond1;
int mozeNit1 = 1;
pthread_cond_t cond2;
int mozeNit2 = 0;

void * nit1(void * arg) {

	pthread_mutex_lock(&lock);

	for (int i = 0; i < 26; ++i)  {
		while (!mozeNit1)
			pthread_cond_wait(&cond1, &lock);
		printf("%c", 'a' + i);
		mozeNit2 = 1;
		mozeNit1 = 0;
		pthread_cond_signal(&cond2);
	}

	pthread_mutex_unlock(&lock);
}

void * nit2(void * arg) {
	
	pthread_mutex_lock(&lock);

	for (char i = 'A'; i <= 'Z'; ++i)  {
		while (!mozeNit2)
			pthread_cond_wait(&cond2, &lock);
		printf("%c", i);
		mozeNit2 = 0;
		mozeNit1 = 1;
		pthread_cond_signal(&cond1);
	}

	pthread_mutex_unlock(&lock);
}

int main(int argc, char * argv[]) {

	pthread_t t1, t2;

	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&cond1, NULL);
	pthread_cond_init(&cond2, NULL);

	pthread_create(&t1, NULL, (void*)nit1, NULL);
	pthread_create(&t2, NULL, (void*)nit2, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("\n");

	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond1);
	pthread_cond_destroy(&cond2);

	return 0;
}