#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "net_errno.h"
#include "net_structs.h"

double *netfpass(struct neuronet *net, double *data)
{
	int i, j, k;
	double *inp, *out;
	double *w;
	
	if ((out = (double *)malloc(sizeof(double *) * net->total_nn)) == NULL) {
		net_errno = NET_ENOMEM;
		out = NULL;
		goto exit_failure;
	}
		
	w = net->w;
	inp = data;

	for (i = 0; i < net->nl; i++) {
		for (j = 0; j < net->nn[i]; j++) {
			double sum = 0.0;
			int nw = net->nw[i];
			for (k = 0; k < nw; k++) 
				sum += (*w++) * inp[k];
			*out++ = 1.0 / (1.0 + exp(-sum));
		}
 
		inp = out - net->nn[i];
	}
	
	out -= net->total_nn;

exit_failure:
	return out;
}

int netbpass(struct neuronet *net, double *out, double *target, double eta)
{
	double *errors, *w;
	int i, j, k, ne, nl, nn, nw;
	
	nl = net->nl;
	ne = net->nn[nl - 1];
	target += ne - 1;
	if ((errors = (double *)malloc(sizeof(double) * ne)) == NULL) {
		net_errno = NET_ENOMEM;
		goto exit_failure;
	}


exit_failure:
	return -1;
}
/*
void netbpass(struct neuronet *net, double **out, double *target, double eta)
{
	double *errors, *w;
	int i, j, k, ne, nl, nn, nw;
	
	nl = net->nl;
	ne = net->nn[nl - 1];
	errors = (double *)malloc(sizeof(double) * ne);

	for (i = 0; i < ne; i++) {
		errors[i] = out[nl - 1][i] * (1.0 - out[nl - 1][i]) * (target[i] - out[nl - 1][i]);
	}

	for (i = nl - 1; i >= 0; i--) {
		nn = net->nn[i];
		nw = net->nw[i];
		w = net->w[i];
		
		for (j = 0; j < nn; j++) 
			for (k = 0; k < nw; k++)
				w[j * nw + k] += eta * errors[j] * out[i][j];
		
		if (i > 0) {
			double *newerrors, newne;
			newne = net->nn[i - 1];
			newerrors = (double *)malloc(sizeof(double) * newne);

			for (j = 0; j < newne; j++) {
				double sum = 0.0;
				for (k = 0; k < ne; k++) 
					sum += w[k * nw + j] * errors[k];
				
				newerrors[j] = out[i - 1][j] * (1.0 - out[i - 1][j]) * sum;
			}
			free(errors);
			ne = newne;
			errors = newerrors;
		}
	}
	free(errors);
}*/
