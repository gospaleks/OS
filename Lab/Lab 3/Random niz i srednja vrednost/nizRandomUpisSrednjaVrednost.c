#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*uradjeno pomocu 1 semafora*/

#define N 5

double niz[N];

sem_t srednjaVr;

void stampajNiz(double* niz, char* poruka) {
  printf("niz %s\n", poruka);
  for (int i = 0; i < N; i++)
    printf("%.2f\t", niz[i]);
  printf("\n");
}

void* nit1(void* arg) {
  for (int i = 0; i < N; i++)
    if (niz[i] < 0)
      niz[i] = abs(niz[i]);

  stampajNiz(niz, "posle apsolutnih vr:");

  sem_post(&srednjaVr);
}

void* nit2(void* arg) {
  sem_wait(&srednjaVr);

  double suma = 0;

  for (int i = 0; i < N; i++)
    suma += niz[i];
  niz[0] = suma / N;
}

int main() {
  srand(time(NULL));

  for (int i = 0; i < N; i++)
    niz[i] = (rand() % 20 - 9);
  stampajNiz(niz, "pre pokretanja niti:");

  pthread_t t1, t2;

  sem_init(&srednjaVr, 0, 0);

  pthread_create(&t1, NULL, (void*)nit1, NULL);
  pthread_create(&t2, NULL, (void*)nit2, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  stampajNiz(niz, "nakon pormene:");

  sem_destroy(&srednjaVr);

  return 0;
}