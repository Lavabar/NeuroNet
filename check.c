#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include "iplimage.h"
#include "iplvideo.h"
#include "ipldefs.h"
#include "netpass.h"
#include "net_structs.h"
#include "netcreat.h"
#include "netfile.h"
#include "edge_detect.h"

#define NEURO_PATH "/home/user/NeuroNet/neuro.data"

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
	int i;
	struct IplImage *frame;
	double *out, *data;
	struct neuronet *net;

	net = (struct neuronet *)malloc(sizeof(struct neuronet));
	if (netfromfile(net, NEURO_PATH) == -1) {
		fprintf(stderr, "error reading nets\n");
		goto exit_failure;
	}
	
	if ((frame = ipl_readimg("/home/user/img2.png", IPL_RGB_MODE)) == NULL) {
		fprintf(stderr, "error reading image\n");
		goto exit_failure;
	}
	//int thres;
	//for (thres = 50; thres < 300; thres += 50) {
		//printf("thres = %d\n", thres);
		//data = sobel(frame, thres);
		data = getdata(frame);
		out = netfpass(net, data);
		out += net->total_nn - 11;
		for (i = 0; i < net->nn[net->nl - 1]; i++)		
			printf("out%d = %lf\n", i, *(out + i));
		out -= net->total_nn - 11;
		free(out);	
		printf("\n\n");
	//}
	return 0;

exit_failure:
	return -1;

}
