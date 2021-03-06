#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "netcreat.h"
#include "net_structs.h"
#include "netpass.h"
#include "netfile.h"
#include "net_errno.h"
#include "ipldefs.h"
#include "iplimage.h"
#include "edge_detect.h"

#define NEURO_PATH "/home/user/NeuroNet1/neuro.data"
#define SAMPLE_PATH "/home/user/Sources/t10k-images.idx3-ubyte"
#define LABEL_PATH "/home/user/Sources/t10k-labels.idx1-ubyte"
#define SAMPLE_CNT 10000	
#define SAMPLE_WIDTH 28
#define SAMPLE_HEIGHT 28
#define SAMPLE_SIZE (SAMPLE_WIDTH * SAMPLE_HEIGHT)

#define ETA 0.005

struct sample {
	double *data;
	double target[10];
};

static void shufflearr(int *pathidx, int len)
{
	int n1, n2, tmp;

	n1 = rand() % (len);
	n2 = rand() % (len);

	while (n1 == n2) 
		n2 = rand() % (len);
 
	tmp = *(pathidx + n1);
	*(pathidx + n1) = *(pathidx + n2);
	*(pathidx + n2) = tmp;
}

/*static double *getdata(struct IplImage *img)
{
	int x, y;
	double *data;
	
	data = (double *)malloc(sizeof(double) * img->width * img->height);
	for (y = 0; y < img->height; y++)
		for (x = 0; x < img->width; x++) {
			unsigned char r, g, b, max;
			r = img->data[img->nchans * (y * img->width + x) + 0];
			g = img->data[img->nchans * (y * img->width + x) + 1];
			b = img->data[img->nchans * (y * img->width + x) + 2];
			max = (r > g)? r : g;
			max = (b > max)? b : max;
			data[y * img->width + x] = (double)max / 255.0 * 2.0 - 1.0;
		}
	return data;
}*/


int main(int argc, char** argv)
{
	int nl = 2;
	int nn[] = {1000, 10};
	double *out;	
	struct sample *examples;
	int *idxes;	
	FILE *f_labels, *f_samples;
	int samples_got[10];
	int i, j;
	unsigned int nrows = 0, ncols = 0, nimgs = 0, magic_number = 0;	
//	char name[256];
	struct neuronet *net = malloc(sizeof(struct neuronet));	
	
	f_labels = fopen(LABEL_PATH, "r");	
	f_samples = fopen(SAMPLE_PATH, "r");	

	fread(&magic_number, 1, 1, f_labels);
	magic_number <<= 8;
	fread(&magic_number, 1, 1, f_labels);
	magic_number <<= 8;
	fread(&magic_number, 1, 1, f_labels);
	magic_number <<= 8;
	fread(&magic_number, 1, 1, f_labels);

	fread(&nimgs, 1, 1, f_labels);
	nimgs <<= 8;
	fread(&nimgs, 1, 1, f_labels);
	nimgs <<= 8;
	fread(&nimgs, 1, 1, f_labels);
	nimgs <<= 8;
	fread(&nimgs, 1, 1, f_labels);

	printf("magic_number=%x nimgs=%x\n", magic_number, nimgs);
	printf("magic_number=%d nimgs=%d\n", magic_number, nimgs);

	
	fread(&magic_number, 1, 1, f_samples);
	magic_number <<= 8;
	fread(&magic_number, 1, 1, f_samples);
	magic_number <<= 8;
	fread(&magic_number, 1, 1, f_samples);
	magic_number <<= 8;
	fread(&magic_number, 1, 1, f_samples);

	fread(&nimgs, 1, 1, f_samples);
	nimgs <<= 8;
	fread(&nimgs, 1, 1, f_samples);
	nimgs <<= 8;
	fread(&nimgs, 1, 1, f_samples);
	nimgs <<= 8;
	fread(&nimgs, 1, 1, f_samples);

	printf("magic_number=%x nimgs=%x\n", magic_number, nimgs);
	printf("magic_number=%d nimgs=%d\n", magic_number, nimgs);


	fread(&nrows, 1, 1, f_samples);
	nrows <<= 8;
	fread(&nrows, 1, 1, f_samples);
	nrows <<= 8;
	fread(&nrows, 1, 1, f_samples);
	nrows <<= 8;
	fread(&nrows, 1, 1, f_samples);
	
	fread(&ncols, 1, 1, f_samples);
	ncols <<= 8;
	fread(&ncols, 1, 1, f_samples);
	ncols <<= 8;
	fread(&ncols, 1, 1, f_samples);
	ncols <<= 8;
	fread(&ncols, 1, 1, f_samples);

	printf("nrows=%x nncols=%x\n", nrows, ncols);
	printf("nrows=%d nncols=%d\n", nrows, ncols);

	examples = (struct sample *)malloc(sizeof(struct sample) * nimgs);
	idxes = (int *)malloc(sizeof(int) * nimgs);

	if (netfromfile(net, NEURO_PATH) == -1)
		net = netcreat(nl, nn, nrows * ncols);


	bzero(samples_got, sizeof(*samples_got) * 10);
	for (i = 0; i < nimgs; i++) {
		unsigned char type;
		unsigned char *data;
	//	FILE *f_tmp;
	//	char name[255];

		fread(&type, sizeof(type), 1, f_labels);
		samples_got[type]++;

		data = malloc(sizeof(*data) * nrows * ncols);
		fread(data, 1, nrows * ncols, f_samples);
		
	/*	bzero(name, 255);
		sprintf(name, "/home/vadim/Sources/tmp/%d/%d.pgm", type, samples_got[type]);
		f_tmp = fopen(name, "w+");
		
		fprintf(f_tmp, "P2\n");
		fprintf(f_tmp, "%d %d\n", ncols, nrows);
		fprintf(f_tmp, "255\n");
		
		for (j = 0; j < nrows * ncols; j++)
			fprintf(f_tmp, "%d\n", data[j]);

		fclose(f_tmp);*/

		examples[i].data = malloc(sizeof(*examples[i].data) * nrows * ncols);
		bzero(examples[i].target, 10 * sizeof(double));
		examples[i].target[type] = 1.0;
		for (j = 0; j < nrows * ncols; j++)
			examples[i].data[j] = (double)data[j] / 255.0 * 2.0 - 1.0;
		idxes[i] = i;
	}
	
	double error;
	int u = 0;
	error = 0.0;
	for (i = 0; i < nimgs; i++) {
		out = netfpass(net, (examples + i)->data);			
	
		out += net->total_nn - 11;
	
		int idx, max;
		max = 0;
		idx = 0;
		for (j = 0; j < net->nn[nl - 1]; j++) {
			error += fabs(out[j] - (examples + i)->target[j]);
			if (out[j] > max) {
				max = out[j];
				idx = j;
			}
		}
		if ((examples + i)->target[idx] == 1.0)
			u++;
		
		out -= net->total_nn - 11;
		free(out);
	}
	printf("%d is correct\n", u);
	printf("error = %lf\n", (error / SAMPLE_CNT / 10));
		
	return 0;

//exit_failure:
//	return -1;
}
