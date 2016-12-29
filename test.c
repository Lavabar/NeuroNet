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

#define SAMPLE_MARK 5 //dog
#define SAMPLE_PATH "/home/user/ConvNet/cifar/test_batch.bin"
#define SAMPLE_CNT 10000	
#define SAMPLE_WIDTH 32
#define SAMPLE_HEIGHT 32
#define SAMPLE_SIZE (SAMPLE_WIDTH * SAMPLE_HEIGHT)

#define ETA 0.05

#define N_CONV_LAYERS 1
#define N_KERNELS 3   
#define KERNEL_WIDTH 7

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
	FILE *f;
	char name[256];
	double *out;
	struct neuronet *net;
	struct sample *examples;
	int *idxes;
	int i, j, k, g, t, u;
	int corr, incorr;

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

	examples = (struct sample *)malloc(sizeof(struct sample) * SAMPLE_CNT);
	idxes = (int *)malloc(sizeof(int) * SAMPLE_CNT);	
	u = 0;	
	bzero(name, 256);
	sprintf(name, "%s", SAMPLE_PATH);
	f = fopen(name, "r");
	for (i = 0; i < (SAMPLE_CNT); i++) {
		unsigned char n;
		unsigned char *a;
		struct IplImage *img;
		img = ipl_creatimg(SAMPLE_WIDTH, SAMPLE_HEIGHT, IPL_RGB_MODE);
		fread(&n, sizeof(unsigned char), 1, f);
		a = (unsigned char *)malloc(sizeof(unsigned char) * SAMPLE_SIZE * img->nchans);
		if (fread(a, sizeof(unsigned char), SAMPLE_SIZE * img->nchans, f) < SAMPLE_SIZE * img->nchans) {
			printf("not read\n");
			exit(1);
		}
		for (k = 0, g = 0; k < SAMPLE_SIZE; k++, g += 3) {
			img->data[g + 0] = a[k];
			img->data[g + 1] = a[k + 1024];
			img->data[g + 2] = a[k + 2048];
		}
		(examples + i)->data = getdata(img);
		if (n == SAMPLE_MARK) {
			u++;
			(examples + i)->target[0] = 1.0;	
			(examples + i)->target[1] = 0.0;
		} else {
			(examples + i)->target[0] = 0.0;	
			(examples + i)->target[1] = 1.0;
		}
		*(idxes + i) = i;
		ipl_freeimg(&img);
		free(a);
	}
	fclose(f);
	int x, y;
	
	/*for (y = 0; y < imgs->h; y++) {
		for (x = 0; x < imgs->w; x++)
			printf("%.2lf ", imgs[0].data[y * imgs->w + x]);
		printf("\n");
	}*/
	printf("real_val =  %d\nreal_inval = %d\n", u, (SAMPLE_CNT - u));
	double error;
	time_t start, end;
	start = time(NULL);
		error = 0;
		corr = 0;
		for (i = 0; i < SAMPLE_CNT; i++) {
			int idx;
			double val, inval;
			idx = *(idxes + i);
			out = netfpass(net, (examples + idx)->data);

			val = *(out + net->total_nn - 2);		
			inval = *(out + net->total_nn - 1);		
			if ((examples + i)->target[0] == 1.0 && val >= 0.7 && inval <= 0.3)
				corr++;
			else if ((examples + i)->target[0] == 0.0 && val <= 0.3 && inval >= 0.7)
				corr++;	
			//error += abs(((*((examples + idx)->target) == 1)? isgun_val : isnotgun_val) - *((examples + idx)->target));
			
			if (*((examples + idx)->target) == 1.0)
				error += 1.0 - val;
			if (*((examples + idx)->target) == 0.0)
				error += 1.0 - inval;

			error += fabs((examples + idx)->target[0] - val) + fabs((examples + idx)->target[1] - inval);


			//printf("idx = %d    tar1 = %lf   tar2 = %lf\n", idx,*((examples + idx)->target), *((examples + idx)->target + 1));
			/*ictf (*((examples + idx)->target) == 1.0 && isgun_val >= isnotgun_val)
				isguncor++;
			else if (*((examples + idx)->target) == 1.0 && isgun_val < isnotgun_val)
				notgunincor++;
			else if (*((examples + idx)->target) == 0.0 && isnotgun_val >= isgun_val)
				notguncor++;
			else if (*((examples + idx)->target) == 0.0 && isnotgun_val < isgun_val)
				isgunincor++;
			*/
					//getchar();	
			free(out);
			/*w = net->w;
			for (i = 0; i < net->nl; i++) {
				for(j = 0; j < net->nn[i]; j++) {
					for(k = 0; k < net->nw[i]; k++)
						printf("%lf|", *w++);
					printf(" ");
				}
				printf("\n");
			}*/
			
		}
		printf("error = %lf, corr = %d\n", (error / SAMPLE_CNT / 2), corr);
		//printf("isguncor = %d isgunincor = %d    notguncor = %d  notgunincor = %d\n", isguncor, isgunincor, notguncor, notgunincor);
		//getchar();

	end = time(NULL);
	printf("%ti\n", end - start);
	return 0;
	
exit_failure:
	return -1;	
}
