#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
	Preko argumenata prima N brojeva i izracunava njihovu srednju vrednost
*/

int main(int argc, char * argv[]) {

	double suma = 0.0;

	for (int i = 1; i < argc; ++i) {
		double num = atof(argv[i]);
		suma += num;
	}

	printf("Srednja vrednost: %.2lf\n", suma / (argc - 1));

	return 0;
}