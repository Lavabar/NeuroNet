#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <png.h>
#include "iplimage.h"
#include "iplvideo.h"
#include "ipldefs.h"

int main()
{
	int i, j;
	struct IplDev *dev;
	struct IplImage *img;
	FILE *fo;
	char name [256];
	
	if ((dev = ipl_opendev(0, IPL_RGB_MODE)) == NULL)	 {
		printf("error while creating device\n");
		return 1;
	}
	for (j = 0; j < 100; j++) {
		if ((img = ipl_getframe(dev)) == NULL) {
			printf("error capturing frame\n");
			return 1;
		}

		sprintf(name, "/home/vadim/test_cam/test%d.ppm", j);
		fo = fopen(name, "w+");
		fprintf(fo, "P3\n");
		fprintf(fo, "%d %d\n", img->width, img->height);
		fprintf(fo, "255\n");
		
/*	for (y = 0; y < imgs[i]->width; y++) 
		for (x = 0; x < imgs[i]->height; x++) 
			fprintf(fo, "%u\n", imgs[i]->data[(y * imgs[i]->width + x)]);*/

		for (i = 0; i < img->width * img->height * img->nchans; i++)
			fprintf(fo, "%u\n", img->data[i]);
	}

	fclose(fo);	
	return 0;
}
