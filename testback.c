#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net_structs.h"
#include "netpass.h"

int main()
{
	struct neuronet net;
	double **out;
	double input[] = {0.1, 0.7}, target[] = {1.0};
	int nl = 2;
	int nn[] = {2, 1};
	int nw = 2;
	int i, j, k;

	
    net.nl = nl;
    net.nn = (int *)malloc(sizeof(int) * nl);
    net.nw = (int *)malloc(sizeof(int) * nl);
    memcpy(net.nn, nn, sizeof(int) * nl);
    
	net.nw[0] = nw;
    for (i = 1; i < nl; i++)
        net.nw[i] = net.nn[i - 1];

    net.w = (double **)malloc(sizeof(double) * nl);
    for (i = 0; i < nl; i++)
        net.w[i] = (double *)malloc(sizeof(double) * nn[i] * net.nw[i]);
    
	net.w[0][0] = 0.1;		
	net.w[0][1] = 0.5;
			
	net.w[0][2] = 0.3;		
	net.w[0][3] = 0.2;
			
	net.w[1][0] = 0.2;		
	net.w[1][1] = 0.1;	
	
	out = netfpass(&net, input);
	
	for (i = 0; i < nl; i++) {
		printf("%d: ", i);
		for (j = 0; j < nn[i]; j++)
			printf("%lf, ", out[i][j]);	
		printf("\n");
	}
	
	netbpass(&net, out, target, 1.0);
	
	for (i = 0; i < nl; i++) {
		printf("%d: ", i);
		for (j = 0; j < nn[i]; j++)
			for (k = 0; k < net.nw[i]; k++)
				printf("%lf, ", net.w[i][j * nn[i] + k]);
		printf("\n");
	}
	return 0;
}
