#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "netcreat.h"
#include "net_structs.h"
#include "netpass.h"
#include "netfile.h"
#include "net_errno.h"
#include "ipldefs.h"
#include "iplimage.h"
#include "edge_detect.h"
#include "draw.h"

#define NEURO_PATH "/home/user/NeuroNet/neuro.data"

static double *getdata(struct IplImage *img, int sx, int sy, int dw, int dh)
{
	int x, y, x1, y1;
	double *data;
	
	data = (double *)malloc(sizeof(double) * dw * dh);
	for (y = sy, y1 = 0; y < sy + dh; y++, y1++)
		for (x = sx, x1 = 0; x < sx + dw; x++, x1++) {
			unsigned char r, g, b, max;
			r = img->data[img->nchans * (y * img->width + x) + 0];
			g = img->data[img->nchans * (y * img->width + x) + 1];
			b = img->data[img->nchans * (y * img->width + x) + 2];
			max = (r > g)? r : g;
			max = (b > max)? b : max;
			data[y1 * dw + x1] = (double)max / 255.0 * 2.0 - 1.0;
		}
	return data;
}

int neurowork(struct IplImage *frame)
{
	int stat;
	//int nl = 2;
	//int nn[] = {100, 2};
	struct neuronet *net = malloc(sizeof(struct neuronet));

	int x, y;

	struct IplImage *img;

	if ((stat = netfromfile(net, NEURO_PATH)) == -1) {			// read net from file or create new
		fprintf(stderr, "Can not open file %s: %s\n", strerror(errno), NEURO_PATH);
		goto exit_failure;
	}
	
	img = frame;
	int k = 0;

	while (img->width >= 20 && img->height >= 50) {
		k++;
		double isgun_val, isnotgun_val;
		printf("w=%d h=%d\n", img->width, img->height);
		for (y = 0; y < img->height - 50; y++) {
			for (x = 0; x < img->width - 20; x++) {
				double *out, *data;
				data = getdata(img, x, y, 20, 50);
				out = netfpass(net, data);
				isgun_val = *(out + net->total_nn - 2);		
				isnotgun_val = *(out + net->total_nn - 1);
				if (isgun_val >= isnotgun_val)
					drawRectangle(frame, x / pow(0.98, k), y / pow(0.98, k), img->width, img->height);
				
				free(out);
				free(data);
			}
		//	printf("height=%d y=%d\n", img->height, y);
		}
		ipl_scaleimg(&img, img->width * 0.98, img->height * 0.98);
	}

	return 0;
	
exit_failure:
	return -1;

}	


