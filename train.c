#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "netcreat.h"
#include "net_structs.h"
#include "netpass.h"
#include "netfile.h"
#include "net_errno.h"
#include "ipldefs.h"
#include "iplimage.h"
#include "edge_detect.h"

#define NEURO_PATH "/home/user/NeuroNet/neuro.data"
#define GUNS_PATH "/home/user/NeuroNet/guns"
#define NOTGUNS_PATH "/home/user/NeuroNet/notguns"

#define CNTGUNS 57	
#define CNTNOTGUNS 97 
#define TOTAL (CNTGUNS + CNTNOTGUNS)
#define SAMPLE_SIZE 100
#define ETA 0.01

struct sample {
	double *data;
	double target[2];
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
	int nl = 2;
	int nn[] = {200, 2};
	double *out;
	struct neuronet *net = malloc(sizeof(struct neuronet));

	struct sample *examples;
	int *idxes;	
	
	int i;

	char name[256];
	struct IplImage *img;

	examples = (struct sample *)malloc(sizeof(struct sample) * TOTAL);
	idxes = (int *)malloc(sizeof(int) * TOTAL);	
	
	for (i = 0; i < CNTGUNS; i++) {
		bzero(name, 256);
		sprintf(name, "%s/%d.png", GUNS_PATH, i);
		if ((img = ipl_readimg(name, IPL_RGB_MODE)) == NULL) {
			fprintf(stderr, "error reading image: %s\n", name);
			return 1;
		}
	
		(examples + i)->data = getdata(img);
		(examples + i)->target[0] = 1.0;	
		(examples + i)->target[1] = 0.0;	
		
		*(idxes + i) = i;
		ipl_freeimg(&img);
	}
	for (i = CNTGUNS; i < TOTAL; i++) {
		bzero(name, 256);
		sprintf(name, "%s/%d.png", NOTGUNS_PATH, i - CNTGUNS);
		if ((img = ipl_readimg(name, IPL_RGB_MODE)) == NULL) {
			fprintf(stderr, "error reading image : %s\n", name);
			return 1;
		}
	
		(examples + i)->data = getdata(img);
		(examples + i)->target[0] = 0.0;	
		(examples + i)->target[1] = 1.0;	

		*(idxes + i) = i;
		ipl_freeimg(&img);
	}

	/*for(i = 0; i < TOTAL; i++)
		printf("tar1 = %lf   tar2 = %lf\n", *((examples + i)->target), *((examples + i)->target + 1));
	getchar();*/	

	/*net = netcreat(nl, nn, SAMPLE_SIZE);
	nettofile(net, NEURO_PATH);
	getchar();*/
	
	if(netfromfile(net, NEURO_PATH) == -1) {
		fprintf(stderr, "Can not open file %s: %s\n", strerror(errno), NEURO_PATH);
		goto exit_failure;
	}
			
//	int isguncor, isgunincor;
//	int notguncor, notgunincor;
	double error;
	do { //while(error > 0.018/*isguncor != CNTGUNS || notguncor != CNTNOTGUNS*/) {
		//isguncor = isgunincor = notguncor = notgunincor = 0;
		error = 0;
		for (i = 0; i < TOTAL; i++) {
			int idx;
			double isgun_val, isnotgun_val;
			idx = *(idxes + i);
			out = netfpass(net, (examples + idx)->data);

			isgun_val = *(out + net->total_nn - 2);		
			isnotgun_val = *(out + net->total_nn - 1);		
			
			//error += abs(((*((examples + idx)->target) == 1)? isgun_val : isnotgun_val) - *((examples + idx)->target));
			
			if (*((examples + idx)->target) == 1.0)
				error += 1.0 - isgun_val;
			if (*((examples + idx)->target) == 0.0)
				error += 1.0 - isnotgun_val;


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
		shufflearr(idxes, TOTAL);
		printf("error = %lf\n", (error / TOTAL));
		//printf("isguncor = %d isgunincor = %d    notguncor = %d  notgunincor = %d\n", isguncor, isgunincor, notguncor, notgunincor);
		nettofile(net, NEURO_PATH);

	} while((error / TOTAL) > 0.04);
	

		
	return 0;

exit_failure:
	return -1;
}
