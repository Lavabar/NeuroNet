#include <stdio.h>
#include "netpass.h"
#include "net_structs.h"
#include "netcreat.h"

int main()
{
	int i, j, k;
	struct neuronet *net;
	int ninp = 2;
	int nl = 2;
	int nn[] = {2, 1};
	double target = 0.5;

	net = netcreat(nl, nn, ninp);

	net->w[0] = 0.1;
	net->w[1] = 0.8;
	net->w[2] = 0.4;
	net->w[3] = 0.6;
	net->w[4] = 0.3;
	net->w[5] = 0.9;

	double *out, data[] = {0.35, 0.9};


	out = netfpass(net, data);

/*	for(i = 0; i < net->nl; i++) {
		for(j = 0; j < net->nn[i]; j++)
			printf("%lf ",*out++);
		printf("\n");
	}*/
	//printf("\n\n\n");
	netbpass(net, data, out, &target, 1);
	for (i = 0; i < net->nl; i++) {
		for(j = 0; j < net->nn[i]; j++) {
			for(k = 0; k < net->nw[i]; k++)
				printf("%lf|", *net->w++);
			printf(" ");
		}
		printf("\n");
	}
	
	return 0;
}
