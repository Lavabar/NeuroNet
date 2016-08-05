#include <stdio.h>
#include "netcreat.h"
#include "net_structs.h"

int main()
{
	int nl = 2;
	int nn[] = {3, 1};
	int ninp = 2;
	double *w;
	struct neuronet *net;
	
	int i, j, k;

	if ((net = netcreat(nl, nn, ninp)) == NULL) {
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

//	printf("w=%x\n", net->w); 

	return 0; 
}
