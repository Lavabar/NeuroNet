#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "netcreat.h"
#include "net_structs.h"
#include "netpass.h"
#include "netfile.h"

#define NEURO_PATH "/home/user/NeuroNet/neuro.data"

int main(int argc, char** argv)
{
	int stat;
	int nl = 2;
	int nn[] = {3, 1};
	int ninp = 2;
	double target = 1.0;
	double *out, *w;
	struct neuronet *net = malloc(sizeof(struct neuronet));

	double samples[] = {0.35, 0.9};	
	
	int i, j, k;

	if ((stat = netfromfile(net, NEURO_PATH)) == 1) {	
		if ((net = netcreat(nl, nn, ninp)) == NULL) {
			fprintf(stderr, "error\n");
			goto exit_failure;
		}
	} else if (stat == -1) {
		fprintf(stderr, "Can not open file %s: %s\n", strerror(errno), NEURO_PATH);
		goto exit_failure;
	}
		
	if (argc == 1 || (argc > 1 && argv[1][0] != 'b')) {
		out = netfpass(net, samples);
		for(i = 0; i < net->nl; i++) {
			for(j = 0; j < net->nn[i]; j++)
				printf("%lf ",*out++);
			printf("\n");
		}		
	} else {
		out = netfpass(net, samples);
		netbpass(net, samples, out, &target, 1);
		w = net->w;
		for (i = 0; i < net->nl; i++) {
			for(j = 0; j < net->nn[i]; j++) {
				for(k = 0; k < net->nw[i]; k++)
					printf("%lf|", *w++);
				printf(" ");
			}
			printf("\n");
		}
	}
	
	nettofile(net, NEURO_PATH);
		
	return 0;

exit_failure:
	return -1;
}
