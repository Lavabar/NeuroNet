#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <string.h>
#include <png.h>
#include "iplimage.h"
#include "iplvideo.h"
#include "ipldefs.h"

#include <math.h>
#include <errno.h>
#include "netcreat.h"
#include "net_structs.h"
#include "netpass.h"
#include "netfile.h"
#include "net_errno.h"
#include "draw.h"

#define NEURO_PATH "/home/user/NeuroNet/neuro.data"
#define SAMPLE_HEIGHT 20
#define SAMPLE_WIDTH 50
#define SCALE_RATE 0.8

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

int neurowork(struct IplImage *frame, struct neuronet *net)
{
	int x, y;
	struct IplImage *img;	
	img = ipl_cloneimg(frame);
	int k = 0;

	double *target[] = {0, 1};

	while (img->width >= 50 && img->height >= 20) {
		double isgun_val, isnotgun_val;
		for (y = 0; y < img->height - 25; y += 25) {
			for (x = 0; x < img->width - 10; x += 10) {
				double *out, *data;
				data = getdata(img, x, y, 50, 20);
				out = netfpass(net, data);
				isgun_val = *(out + net->total_nn - 2);		
				isnotgun_val = *(out + net->total_nn - 1);
				if (isgun_val >= 0.6 && isnotgun_val <= 0.3)
				{
					netbpass(net, data, out, target, 0.01);
					nettofile(net, NEURO_PATH);

				}
				free(out);
				free(data);
			}
		}
		k++;
		ipl_scaleimg(&img, img->width * SCALE_RATE, img->height * SCALE_RATE);
	}
	
	return 0;
}

int main(int argc, char **argv)
{
	struct IplImage *frame;
	struct IplDev *dev1;

	time_t start, end;
	int f;

	struct neuronet *net = malloc(sizeof(struct neuronet));
	if (netfromfile(net, NEURO_PATH) == -1) {			// read net from file or create new
		fprintf(stderr, "Can not open file %s: %s\n", strerror(errno), NEURO_PATH);
		goto exit_failure;
	}

	f = 0;
	start = 0;
	end = 0;
	if ((dev1 = ipl_opendev(0, IPL_RGB_MODE)) == NULL)
		printf("error while creating device 0\n");
	if (ipl_setparams(dev1, 320, 240, IPL_FORCE_SCALE_OFF) < 0) {
		fprintf(stderr, "error on changing cam params\n");
		free(dev1);
	}
	flag = 1;
	start = time(NULL);
	
	while (flag) {
		f++;	
		end = time(NULL);
		if (end - start >= 1.0) {
			printf("fps = %i\n", f);
			f = 0;
			start = end;
		}
	
		if ((frame = ipl_getframe(dev1)) == NULL) {
			printf("error capturing curr1\n");
		}
		ipl_scaleimg(&frame, 640, 480);

		if(neurowork(frame), net) {
			fprintf(stderr, "error in neurowork\n");
			goto exit_failure;
		}
		ipl_freeimg(&frame);
	}

	return 0;

exit_failure:
	return -1;
}
