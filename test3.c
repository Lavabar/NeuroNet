#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <png.h>
#include "iplimage.h"
#include "iplvideo.h"
#include "ipldefs.h"
#include "ipltransform.h"
#include "net_structs.h"
#include "netpass.h"
#include "netfile.h"

#define PATH "/home/vadim/libipl/neuro.data"
#define FACE_PATH "/home/vadim/test_cam/faces/"
#define NONFACE_PATH "/home/vadim/test_cam/nonfaces/"

double *getdata(struct IplImage *img, int sx, int sy, int dw, int dh)
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

int main()
{
	int i, j, k, x, y, count;
	int wstep, hstep;
	struct IplDev *dev;
	struct IplImage *img, *tmp;
	FILE *fo;
	char name [256];
	struct neuronet net;
	double *data;

	netfromfile(&net, PATH);

	if ((dev = ipl_opendev(1, IPL_RGB_MODE)) == NULL)	 {
		printf("error while creating device\n");
		return 1;
	}

	if ((img = ipl_readimg("/home/vadim/putin.png", IPL_RGB_MODE)) == NULL) {
		printf("error capturing frame\n");
		return 1;
	}
	
	bzero(name, 256);
	sprintf(name, "/home/vadim/test_cam/test.ppm");
	fo = fopen(name, "w+");
	fprintf(fo, "P3\n");
	fprintf(fo, "%d %d\n", img->width, img->height);
	fprintf(fo, "255\n");
		
/*	for (y = 0; y < imgs[i]->width; y++) 
		for (x = 0; x < imgs[i]->height; x++) 
			fprintf(fo, "%u\n", imgs[i]->data[(y * imgs[i]->width + x)]);*/

	for (i = 0; i < img->width * img->height * img->nchans; i++)
		fprintf(fo, "%u\n", img->data[i]);
	fclose(fo);
	
	count = 0;
	tmp = ipl_scaleimg(img, 320, 240);
	ipl_freeimg(&img);
	img = tmp;
	wstep = (int)((double)img->width / 20.0);
	hstep = (int)((double)img->height / 20.0);

	//first output for face, second for nonface; 	
	while (img->width >= 50 && img->height >= 50) {
		printf("w=%d h=%d\n", img->width, img->height);
		for (y = 0; y < img->height - 50; y += 50) {
			for (x = 0; x < img->width - 50; x += 50) {
				double **out;
				data = getdata(img, x, y, 50, 50);
				out = netfpass(&net, data);

				bzero(name, 256);
				if (out[net.nl - 1][0] > 0.9) {
					printf("face %lf %lf\n", out[net.nl - 1][0], out[net.nl - 1][1]);
					sprintf(name, "%s/%d.ppm", FACE_PATH, count);
					fo = fopen(name, "w+");
					fprintf(fo, "P3\n");
					fprintf(fo, "50 50\n");
					fprintf(fo, "255\n");
		

					for (i = y; i < y + 50; i++) 
						for (j = x; j < x + 50; j++)
							for (k = 0; k < img->nchans; k++)
								fprintf(fo, "%u\n", img->data[img->nchans * (i * img->width + j) + k]);
					fclose(fo);
					count++;
				}

				for (i = 0; i < net.nl; i++)
					free(out[i]);
				free(out);
				free(data);
			}
		//	printf("height=%d y=%d\n", img->height, y);
		}
		tmp = ipl_scaleimg(img, img->width - wstep, img->height - hstep);
		ipl_freeimg(&img);
		img = tmp;
	}
		
	return 0;
}
