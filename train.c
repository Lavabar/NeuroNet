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

#define CNTGUNS 18
#define CNTNOTGUNS 18
#define TOTAL (CNTGUNS + CNTNOTGUNS)
#define SAMPLE_SIZE 100
#define ETA 1.0

struct sample {
	double *data;
	double target[2];
};

static void shufflearr(int *pathidx, int len)
{
	int p1;
	for (p1 = 0; p1 < len; ++p1) {
		int p2;
		int tmp;
		
		p2 = rand() % len;
		tmp = pathidx[p1];

		pathidx[p1] = pathidx[p2];
		pathidx[p2] = tmp;
	}
}

int main(int argc, char** argv)
{
	int stat;
	int nl = 2;
	int nn[] = {100, 2};
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
	
		(examples + i)->data = sobel(img, 250);
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
	
		(examples + i)->data = sobel(img, 250);
		(examples + i)->target[0] = 0.0;	
		(examples + i)->target[1] = 1.0;	

		*(idxes + i) = i;
		ipl_freeimg(&img);
	}
	
	if ((stat = netfromfile(net, NEURO_PATH)) == 1) {			// read net from file or create new
		if ((net = netcreat(nl, nn, SAMPLE_SIZE)) == NULL) {
			fprintf(stderr, "netcreat: %d\n", net_errno);
			goto exit_failure;
		}
	} else if (stat == -1) {
		fprintf(stderr, "Can not open file %s: %s\n", strerror(errno), NEURO_PATH);
		goto exit_failure;
	}
	
	int isguncor, isgunincor;
	int notguncor, notgunincor;
	while(isguncor != CNTGUNS && notguncor != CNTNOTGUNS) {
		isguncor = isgunincor = notguncor = notgunincor = 0;
		for (i = 0; i < TOTAL; i++) {
			int idx;
			double isgun_val, isnotgun_val;
			idx = *(idxes + i);
			out = netfpass(net, (examples + idx)->data);

			isgun_val = *(out + net->total_nn - 2);		
			isnotgun_val = *(out + net->total_nn - 1);		


			if (*((examples + idx)->target) == 1.0 && isgun_val >= isnotgun_val)
				isguncor++;
			else if (*((examples + idx)->target) == 1.0 && isgun_val < isnotgun_val)
				isgunincor++;
			else if (*((examples + idx)->target) == 0.0 && isnotgun_val >= isgun_val)
				notguncor++;
			else if (*((examples + idx)->target) == 0.0 && isnotgun_val < isgun_val)
				notgunincor++;

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
			shufflearr(idxes, TOTAL);
			
		}
		printf("isguncor = %d isgunincor = %d    notguncor = %d  notgunincor = %d\n", isguncor, isgunincor, notguncor, notgunincor);
		nettofile(net, NEURO_PATH);
	}
	

		
	return 0;

exit_failure:
	return -1;
}
