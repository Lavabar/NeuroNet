#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "net_structs.h"
#include "netcreat.h"
#include "netpass.h"

#define SAMPLE_SIZE 4

int main(void)
{
    int nn[] = {5, 3, 4, 2};
    int nw = SAMPLE_SIZE;
    int nl = 4;
    struct neuronet net;
    int i, j, k;
	double sample[SAMPLE_SIZE];
	double **out;
	
    net = netcreat(nl, nn, nw); 

	srand(time(NULL));
	for (i = 0; i < SAMPLE_SIZE; i++)
		sample[i] = (double)rand() / (double)RAND_MAX;

	out = netfpass(&net, sample);

    for (i = 0; i < net.nl; i++) {
        printf("%i: ", i);
        for (j = 0; j < net.nn[i]; j++){
           // printf("\t%lf\n", net.w[i][j]);
            for (k = 0; k < net.nw[i]; k++)
                printf("%lf ", net.w[i][j * net.nw[i] + k]);
            printf(", ");
        }
        printf("\n");
    }
	printf("\n");
	for (i = 0; i < net.nl; i++) {
		printf("%i", i);
		for (j = 0; j < net.nn[i]; j++)
			printf("%lf ", out[i][j]);
		printf("\n");
	}
    getchar();

    return 0;
}

