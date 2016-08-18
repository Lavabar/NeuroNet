#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "net_errno.h"
#include "net_structs.h"

double *netfpass(struct neuronet *net, double *inp)
{
	int i, j, k;
	double *out;
	double *w;
	
	if ((out = (double *)malloc(sizeof(double *) * net->total_nn)) == NULL) {
		net_errno = NET_ENOMEM;
		out = NULL;
		goto exit_failure;
	}
		
	w = net->w;

	for (i = 0; i < net->nl; i++) {
		for (j = 0; j < net->nn[i]; j++) {
			double sum = 0.0;
			int nw = net->nw[i];
			//printf("w = %lf\n", *w);
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

int netbpass(struct neuronet *net, double *inp, double *out, double *target, double eta)
{
	double *errors, *w;
	int i, j, k, m, ne, nl, nn, nw;
	
	nl = net->nl;
	ne = net->nn[nl - 1];
	target += ne - 1;
	out += net->total_nn - 1;
	w = net->w + net->total_nw - 1;
	inp += net->nw[0] - 1;

	if ((errors = (double *)malloc(sizeof(double) * ne)) == NULL) {
		net_errno = NET_ENOMEM;
		goto exit_failure;
	}
	errors += ne - 1;

	for (i = 0; i < ne; i++, out--, target--) 
		*(errors - i) = (*out) * (1.0 - (*out)) * ((*target) - (*out));
	
	
	for (i = nl - 1; i >= 0; i--) {
		nn = net->nn[i];
		nw = net->nw[i]; 
		if (i > 0) {
			for (j = 0, m = 0; j < nn; j++)  
				for (k = 0; k < nw; k++, m++)
					*(w - m) += eta * *(errors - j) * *(out - k);
		} else {
			for (j = 0, m = 0; j < nn; j++)
				for (k = 0; k < nw; k++, m++) 
					*(w - m) += eta * *(errors - j) * *(inp - k);
		}
		if (i > 0) {
			int newne;
			double *newerrors;

			newne = net->nn[i - 1];
			if ((newerrors = (double *)calloc(newne, sizeof(double))) == NULL) {
				net_errno = NET_ENOMEM;
				goto free_errors;
			}
			newerrors += newne - 1; 
			for (j = 0; j < newne; j++, out--) {
				for (k = 0; k < ne; k++) 
					*(newerrors - j) += *w-- * (*(errors - k));
				
				*(newerrors - j) *= *out * (1.0 - *out);
			}
			free(errors - ne + 1);
			errors = newerrors;
			ne = newne;
		}

	}

	return 0;

free_errors:
	free(errors);
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
