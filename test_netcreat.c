#include <stdio.h>
#include "netcreat.h"
#include "net_structs.h"
#include "netfile.h"

int main()
{
	int nl = 5;
	int nn[] = {400, 200, 100, 50, 10};
	int ninp = 1024;
	double *w;
	struct neuronet *net;
	
	int i, j, k;

	if ((net = netcreat(nl, nn, ninp)) == NULL) {
		fprintf(stderr, "error\n");
		return 1;
	}
	w = net->w;
	nettofile(net, "/home/user/NeuroNet/neuro.data");
//	printf("w=%x\n", net->w); 

	return 0; 
}
