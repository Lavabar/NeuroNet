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
#include "net_def.h"

#define NEURO_PATH "/home/user/NeuroNet/gun_neuro.data"
#define GUNS_PATH "/home/user/NeuroNet/guns"

#define CNTGUNS 156	
#define SAMPLE_SIZE 1000
#define ETA 0.01

struct sample {
	double *data;
	double target;
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
			unsigned char px;
			px = img->data[img->nchans * (y * img->width + x)];
			/*g = img->data[img->nchans * (y * img->width + x) + 1];
			b = img->data[img->nchans * (y * img->width + x) + 2];
			max = (r > g)? r : g;
			max = (b > max)? b : max;*/
			data[y * img->width + x] = (double)px / 255.0 * 2.0 - 1.0;
		}
	return data;
}


int main(int argc, char** argv)
{
	int nl = 2;
	int nn[] = {100, 1};
	double *out;
	
	struct sample *examples;
	int *idxes;	
	
	int i;

	char name[256];
	struct IplImage *img;

	examples = (struct sample *)malloc(sizeof(struct sample) * CNTGUNS);
	idxes = (int *)malloc(sizeof(int) * CNTGUNS);	
	
	for (i = 0; i < CNTGUNS; i++) {
		bzero(name, 256);
		sprintf(name, "%s/%d.png", GUNS_PATH, i);
		if ((img = ipl_readimg(name, IPL_GRAY_MODE)) == NULL) {
			fprintf(stderr, "error reading image: %s\n", name);
			return 1;
		}
	
		(examples + i)->data = getdata(img);
		(examples + i)->target = 1.0;	
		
		*(idxes + i) = i;
		ipl_freeimg(&img);
	}
	
	/*for(i = 0; i < TOTAL; i++)
		printf("tar1 = %lf   tar2 = %lf\n", *((examples + i)->target), *((examples + i)->target + 1));
	getchar();*/	

	/*gun_net = netcreat(nl, nn, SAMPLE_SIZE);
	nettofile(gun_net, NEURO_PATH);
	getchar();*/
	
	gun_net = malloc(sizeof(struct neuronet));
	if (netfromfile(gun_net, NEURO_PATH) == -1) {
		fprintf(stderr, "Can not open file: %s: %s\n", strerror(errno), NEURO_PATH);
		goto exit_failure;
	}
				
//	int isguncor, isgunincor;
//	int notguncor, notgunincor;
	double error;
	do { //while(error > 0.018/*isguncor != CNTGUNS || notguncor != CNTNOTGUNS*/) {
		//isguncor = isgunincor = notguncor = notgunincor = 0;
		error = 0;
		for (i = 0; i < CNTGUNS; i++) {
			int idx;
			double isgun_val;
			idx = *(idxes + i);
			out = netfpass(gun_net, (examples + idx)->data);

			isgun_val = *(out + gun_net->total_nn - 1);		
			
			//error += abs(((*((examples + idx)->target) == 1)? isgun_val : isnotgun_val) - *((examples + idx)->target));
						
			error += fabs((examples + idx)->target - isgun_val);


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
			netbpass(gun_net, (examples + idx)->data, out, &((examples + idx)->target), ETA);
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
		shufflearr(idxes, CNTGUNS);
		printf("error = %lf\n", error / CNTGUNS);
		//printf("isguncor = %d isgunincor = %d    notguncor = %d  notgunincor = %d\n", isguncor, isgunincor, notguncor, notgunincor);
		nettofile(gun_net, NEURO_PATH);

	} while(error / CNTGUNS);
	

		
	return 0;

exit_failure:
	return -1;
}
