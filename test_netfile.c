#include <stdio.h>
#include <stdlib.h>
#include "netcreat.h"
#include "net_structs.h"
#include "netfile.h"

int main()
{
	/*int nl = 2;
	int nn[] = {3, 1};
	int ninp = 2;*/
	double *w;
	struct neuronet *net = malloc(sizeof(struct neuronet));
	
	int i, j, k;

	/*if ((net = netcreat(nl, nn, ninp)) == NULL) {
		fprintf(stderr, "error\n");
		return 1;
	}
	w = net->w;

	for (i = 0; i < nl; i++) {
		for(j = 0; j < net->nn[i]; j++) {
			for(k = 0; k < net->nw[i]; k++)
				printf("%lf|", *w++);
			printf(" ");
		}
		printf("\n");
	}

	printf("\n");
	printf("\n");
	printf("\n");*/


	//nettofile(net, "/home/user/NeuroNet/neuro.data");
	
	netfromfile(net, "/home/user/NeuroNet/neuro.data");	
		
	w = net->w;
	for (i = 0; i < net->nl; i++) {
		for(j = 0; j < net->nn[i]; j++) {
			for(k = 0; k < net->nw[i]; k++)
				printf("%lf|", *w++);
			printf(" ");
		}
		printf("\n");
	}

//	printf("w=%x\n", net->w); 

	return 0; 
}
