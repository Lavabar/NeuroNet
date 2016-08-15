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
#define PICSPATH "/home/user/NeuroNet/pics"

#define CNTPICS 10
#define SAMPLE_SIZE 100

struct sample {
	double *data;
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

int main(int argc, char** argv)
{
	int stat;
	int nl = 2;
	int nn[] = {100, 2};
	double *out;
	struct neuronet *net = malloc(sizeof(struct neuronet));

	struct sample *examples;
	
	int i;

	char name[256];
	struct IplImage *img;

	examples = (struct sample *)malloc(sizeof(struct sample) * CNTPICS);
	
	for (i = 0; i < CNTPICS; i++) {
		bzero(name, 256);
		sprintf(name, "%s/%d.png", PICSPATH, i);
		if ((img = ipl_readimg(name, IPL_RGB_MODE)) == NULL) {
			fprintf(stderr, "error reading image: %s\n", name);
			goto exit_failure;
		}
	
		(examples + i)->data = getdata(img);
		
		ipl_freeimg(&img);
	}
	
	if ((stat = netfromfile(net, NEURO_PATH)) == -1) {			// read net from file or create new
		fprintf(stderr, "Can not open file %s: %s\n", strerror(errno), NEURO_PATH);
		goto exit_failure;
	}
	
	for (i = 0; i < CNTPICS; i++) {
		double isgun_val, isnotgun_val;
		out = netfpass(net, (examples + i)->data);
		
		isgun_val = *(out + net->total_nn - 2);		
		isnotgun_val = *(out + net->total_nn - 1);		

		if (isgun_val >= isnotgun_val)
			printf("%d is gun\n", i);
		else if (isgun_val < isnotgun_val)
			printf("%d isn't gun\n", i);
		}

	return 0;
	
exit_failure:
	return -1;

}	


