#include "net_errno.h"
#include "net_structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int validate_args(int nl, int *nn, int ninp)
{
	int i, test = 1;
	test = (nl > 0) && (ninp > 0) && (nn != NULL);
	if (!test)
		return 1;
	
	for (i = 0; i < nl; i++)
		test &=  nn[i] > 0;
	
	if (!test)
		return 1;

	return 0;
}

struct neuronet *netcreat(int nl, int *nn, int ninp)
{
    int i, nperc;
	double *w;
    struct neuronet *res;

	if (validate_args(nl, nn, ninp) != 0) {
		net_errno = NET_EINVAL;
		goto exit_failure;
	}
		
	if ((res = malloc(sizeof(struct neuronet))) == NULL) {
		net_errno = NET_ENOMEM;
		goto exit_failure;
	}
    
	res->nl = nl;
    if ((res->nn = (int *)malloc(sizeof(int) * nl)) == NULL) {
		net_errno = NET_ENOMEM;
		goto free_net;
	}
	if ((res->nw = (int *)malloc(sizeof(int) * nl)) == NULL) {
		net_errno = NET_ENOMEM;
		goto free_nn;
	}
 
    memcpy(res->nn, nn, sizeof(int) * nl);
    res->nw[0] = ninp;
    srand(time(NULL));
    for (i = 1; i < nl; i++) 
        res->nw[i] = res->nn[i - 1];

	res->total_nw = 0;
	nperc = ninp;
	for (i = 0; i < nl; i++) {
		res->total_nw += nn[i] * nperc;
		nperc = nn[i];
	}

	res->total_nn = 0;
	for (i = 0; i < nl; i++)
		res->total_nn += nn[i];	

	if((w = (double *)malloc(sizeof(double) * res->total_nw)) == NULL) {
		net_errno = NET_ENOMEM;
		goto free_nw;
	}
	
    for (i = 0; i < res->total_nw; i++)
		*w++ = (double)rand()/(double)RAND_MAX * 2.0 - 1.0;

	res->w = w - res->total_nw;

    return res;

free_nw:
	free(res->nw);
free_nn:
	free(res->nn);
free_net:
	free(res);
exit_failure:
	return NULL;
}
