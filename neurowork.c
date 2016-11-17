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
#include "net_def.h"

#define SAMPLE_HEIGHT 20
#define SAMPLE_WIDTH 50
#define SCALE_RATE 0.70

static double *getdata(struct IplImage *img, int sx, int sy, int dw, int dh)
{
	int x, y, x1, y1;
	double *data;
	
	data = (double *)malloc(sizeof(double) * dw * dh);
	for (y = sy, y1 = 0; y < sy + dh; y++, y1++)
		for (x = sx, x1 = 0; x < sx + dw; x++, x1++) {
			unsigned char px;
			px = img->data[img->nchans * (y * img->width + x)];
			/*g = img->data[img->nchans * (y * img->width + x) + 1];
			b = img->data[img->nchans * (y * img->width + x) + 2];
			max = (r > g)? r : g;
			max = (b > max)? b : max;*/
			data[y1 * dw + x1] = (double)px / 255.0 * 2.0 - 1.0;
		}
	return data;
}

int neurowork(struct IplImage *frame)
{
	int x, y;
	struct IplImage *img;
	int k = 0;

	double *gun_out, *notgun_out, *data;
	if((img = ipl_cloneimg(frame)) == NULL)
		goto exit_failure;

	while (img->width >= 50 && img->height >= 20) {
		double isgun_val, isnotgun_val;
		//printf("w=%d h=%d\n", img->width, img->height);
		for (y = 0; y < img->height - 20; y+=20) {
			for (x = 0; x < img->width - 50; x+=50) {
				data = getdata(img, x, y, 50, 20);
				gun_out = netfpass(gun_net, data);
				notgun_out = netfpass(notgun_net, data);
				isgun_val = *(gun_out + gun_net->total_nn - 1);		
				isnotgun_val = *(notgun_out + notgun_net->total_nn - 1);
				//printf("isgun_val = %lf    |   isnotgun_val = %lf\n", isgun_val, isnotgun_val);
				if (isgun_val >= 0.7 && isnotgun_val <= 0.3)
				{
					//printf("here\n");
					//printf("x = %d | y = %d     w = %d | h = %d     k = %d\n", (int)(x / pow(SCALE_RATE, k)), (int)(y / pow(SCALE_RATE, k)), (int)(SAMPLE_WIDTH / pow(SCALE_RATE, k)), (int)(SAMPLE_HEIGHT / pow(SCALE_RATE, k)), k);
					int x1, x2;
					int y1, y2;
					x1 = x / pow(SCALE_RATE, k);
					y1 = y / pow(SCALE_RATE, k);
					x2 = x1 + SAMPLE_WIDTH / pow(SCALE_RATE, k);
					y2 = y1 + SAMPLE_HEIGHT / pow(SCALE_RATE, k); 
					drawRectangle(frame, x1, y1, x2, y2);
				}
				free(gun_out);
				free(notgun_out);
				free(data);
			}
		//	printf("height=%d y=%d\n", img->height, y);
		}
		k++;
		ipl_scaleimg(&img, img->width * SCALE_RATE, img->height * SCALE_RATE);
	}

	//printf("\nframe\n");
	return 0;
	
exit_failure:
	return -1;

}	


