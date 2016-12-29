#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include "netpass.h"
#include "netfile.h"
#include "iplimage.h"
#include "ipldefs.h"
#include "netcreat.h"
#include <time.h>

#define NEURO_PATH "/home/user/NeuroNet/neuro.data"

#define SAMPLE_WIDTH 32
#define SAMPLE_HEIGHT 32
#define SAMPLE_SIZE (SAMPLE_WIDTH * SAMPLE_HEIGHT)

#define ETA 0.05

struct sample {
	double *data;
	double target[2];
};

static double *getdata(struct IplImage *img)
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
}

int main()
{
	double *out;
	struct neuronet *net;

	net = (struct neuronet *)malloc(sizeof(struct neuronet));
	/*
	int nl = 1;
	int nn[] = {2};	
	if((cnet = cnetcreat(N_CONV_LAYERS, N_KERNELS, KERNEL_WIDTH)) == NULL) {
		fprintf(stderr, "error in init convnet\n");
		goto exit_failure;
	}
	net = netcreat(nl, nn, ((SAMPLE_WIDTH - ((KERNEL_WIDTH / 2) * 2)) / 2) * ((SAMPLE_HEIGHT - ((KERNEL_WIDTH / 2) * 2)) / 2) * N_KERNELS);
	nettofile(net, cnet, NEURO_PATH);
	getchar();*/

	if (netfromfile(net, NEURO_PATH) == -1) {
		fprintf(stderr, "error reading nets\n");
		goto exit_failure;
	}
	struct IplImage *img;
	img = ipl_readimg("/home/user/dog3.png", IPL_RGB_MODE);
	double *data;
	data = getdata(img);
	double val, inval;
	out = netfpass(net, data);

	val = *(out + net->total_nn - 2);		
	inval = *(out + net->total_nn - 1);		
	//error += abs(((*((examples + idx)->target) == 1)? isgun_val : isnotgun_val) - *((examples + idx)->target));

	printf("it is a dog on %lf\nit is not dog on %lf\n", val * 100, inval * 100);
			
	free(out);
	
	return 0;
	
exit_failure:
	return -1;	
}
