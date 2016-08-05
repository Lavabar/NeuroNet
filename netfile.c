#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net_structs.h"

void nettofile(struct neuronet *net, char *path)
{
	int i, j;
	FILE *f;
	
	if ((f = fopen(path, "w+")) == NULL) {
		fprintf(stderr, "Can not open file %s: %s\n", path, strerror(errno));
		return;
	}

	fprintf(f, "%d\n", net->nl);
	for (i = 0; i < net->nl; i++)	
		fprintf(f, "%d\n", net->nn[i]);

	for (i = 0; i < net->nl; i++)	
		fprintf(f, "%d\n", net->nw[i]);

	for (i = 0; i < net->nl; i++)
		for (j = 0; j < net->nn[i] * net->nw[i]; j++)
			fprintf(f, "%lf\n", net->w[i][j]);
	
	fclose(f);
}

void netfromfile(struct neuronet *net, char *path)
{
	int i, j;
	FILE *f;

	if ((f = fopen(path, "r")) == NULL) {
		fprintf(stderr, "Can not open file %s: %s\n", strerror(errno), path);
		return;
	}

	fscanf(f, "%d\n", &(net->nl));
	printf("nl = %d\n", net->nl);

	net->nn = (int *)malloc(sizeof(int) * net->nl);
	net->nw = (int *)malloc(sizeof(int) * net->nl);
	net->w = (double **)malloc(sizeof(double *) * net->nl);

	for (i = 0; i < net->nl; i++)
		fscanf(f, "%d\n", &(net->nn[i]));

	for (i = 0; i < net->nl; i++)
		fscanf(f, "%d\n", &(net->nw[i]));

	for (i = 0; i < net->nl; i++) {
		net->w[i] = (double *)malloc(sizeof(double) * net->nn[i] * net->nw[i]);
		for (j = 0; j < net->nn[i] * net->nw[i]; j++)
			fscanf(f, "%lf\n", &(net->w[i][j]));
	}

	fclose(f);
}
