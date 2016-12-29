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

#define NEURO_PATH "/home/user/NeuroNet/neuro.data"

#define SAMPLE_PATH "/home/user/ConvNet/cifar/data_batch_"
#define SAMPLE_CNT 50000	
#define SAMPLE_WIDTH 32
#define SAMPLE_HEIGHT 32
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


int main(int argc, char** argv)
{
	int nl = 5;
	int nn[] = {400, 200, 100, 50, 10};
	double *out;
	
	struct sample *examples;
	int *idxes;	
	FILE *f;
	int i, j, k, g, t, u;

	char name[256];
	struct neuronet *net = malloc(sizeof(struct neuronet));
	if (netfromfile(net, NEURO_PATH) == -1) {
		fprintf(stderr, "Can not open file %s: %s\n", strerror(errno), NEURO_PATH);
		goto exit_failure;
	}
	//printf("%d\n", net->nl);

	examples = (struct sample *)malloc(sizeof(struct sample) * SAMPLE_CNT);
	idxes = (int *)malloc(sizeof(int) * SAMPLE_CNT);	
	
	u = 0;	
	for (j = 0, t = 0; j < 5; j++) {
		bzero(name, 256);
		sprintf(name, "%s%d.bin", SAMPLE_PATH, j + 1);
		f = fopen(name, "r");
		for (i = 0; i < (SAMPLE_CNT / 5); i++, t++) {
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
			bzero((examples + t)->target, 10);
			(examples + t)->data = getdata(img);
			(examples + t)->target[n] = 1.0;	
		
			*(idxes + t) = t;
			ipl_freeimg(&img);
			free(a);
		}
		fclose(f);
	}

	/*for(i = 0; i < TOTAL; i++)
		printf("tar1 = %lf   tar2 = %lf\n", *((examples + i)->target), *((examples + i)->target + 1));
	getchar();*/	

	/*net = netcreat(nl, nn, SAMPLE_SIZE);
	nettofile(net, NEURO_PATH);
	getchar();*/
	
				
//	int isguncor, isgunincor;
//	int notguncor, notgunincor;
	double error;
	do { //while(error > 0.018/*isguncor != CNTGUNS || notguncor != CNTNOTGUNS*/) {
		//isguncor = isgunincor = notguncor = notgunincor = 0;
		error = 0;
		for (i = 0; i < SAMPLE_CNT; i++) {
			int idx;
			double isgun_val, isnotgun_val;
			idx = *(idxes + i);
			out = netfpass(net, (examples + idx)->data);			
			
			/*isgun_val = *(out + net->total_nn - 2);		
			isnotgun_val = *(out + net->total_nn - 1);		

			//error += abs(((*((examples + idx)->target) == 1)? isgun_val : isnotgun_val) - *((examples + idx)->target));
			
			if (*((examples + idx)->target) == 1.0)
				error += 1.0 - isgun_val;
			if (*((examples + idx)->target) == 0.0)
				error += 1.0 - isnotgun_val;

			error += fabs((examples + idx)->target[0] - isgun_val) + fabs((examples + idx)->target[1] - isnotgun_val);
			*/

			//printf("idx = %d    tar1 = %lf   tar2 = %lf\n", idx,*((examples + idx)->target), *((examples + idx)->target + 1));
			/*if (*((examples + idx)->target) == 1.0 && isgun_val >= isnotgun_val)
				isguncor++;
			else if (*((examples + idx)->target) == 1.0 && isgun_val < isnotgun_val)
				notgunincor++;
			else if (*((examples + idx)->target) == 0.0 && isnotgun_val >= isgun_val)
				notguncor++;
			else if (*((examples + idx)->target) == 0.0 && isnotgun_val < isgun_val)
				isgunincor++;
			*/
			netbpass(net, (examples + idx)->data, out, (examples + idx)->target, ETA);
			out += net->total_nn - 11;
			for (j = 0; j < net->nn[nl - 1]; j++)
				error += fabs(out[j] - (examples + idx)->target[j]);
			out -= net->total_nn - 11;
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
		shufflearr(idxes, SAMPLE_CNT);
		printf("error = %lf\n", (error / SAMPLE_CNT / 10));
		//printf("isguncor = %d isgunincor = %d    notguncor = %d  notgunincor = %d\n", isguncor, isgunincor, notguncor, notgunincor);
		nettofile(net, NEURO_PATH);

	} while((error / SAMPLE_CNT / 10) > 0.1);
	

		
	return 0;

exit_failure:
	return -1;
}
