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
#include "net_def.h"
#include "neurowork.h"

#define GUN_NEURO_PATH "/home/user/NeuroNet/gun_neuro.data"
#define NOTGUN_NEURO_PATH "/home/user/NeuroNet/notgun_neuro.data"

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

	gun_net = (struct neuronet *)malloc(sizeof(struct neuronet));
	notgun_net = (struct neuronet *)malloc(sizeof(struct neuronet));
	if (netfromfile(gun_net, GUN_NEURO_PATH) == -1) {
		fprintf(stderr, "Can not open file %s: %s\n", strerror(errno), GUN_NEURO_PATH);
		return -1;
	}
	if (netfromfile(notgun_net, NOTGUN_NEURO_PATH) == -1) {
		fprintf(stderr, "Can not open file %s: %s\n", strerror(errno), NOTGUN_NEURO_PATH);
		return -1;
	}


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
