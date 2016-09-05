#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <png.h>
#include "iplimage.h"
#include "iplvideo.h"
#include "ipldefs.h"
#include "hough.h"

/*static int compar(const void *val1, const void *val2)
{
	double a, b;
 
	a = (double)*val1;
	b = (double)*val2;
	if (a > b) return 1;
	else if (a < b) return -1;
	else return 0;
}*/

int main()
{
	double *accarr;
	struct IplImage *frame;
	//struct IplDev *dev1;
	FILE *fo;
	int x, y, r, i;
	int xc, yc;
	double t;
	char name[256];
	
	/*if ((dev1 = ipl_opendev(0, IPL_GRAY_MODE)) == NULL)
		printf("error while creating device 0\n");
	if (ipl_setparams(dev1, 320, 240, IPL_FORCE_SCALE_OFF) < 0) {
		fprintf(stderr, "error on changing cam params\n");
		free(dev1);
	}	
	
	if ((frame = ipl_getframe(dev1)) == NULL)
		printf("error capturing curr1\n");*/

	//ipl_scaleimg(&frame, 640, 480);
	if ((frame = ipl_readimg("/home/user/picture.png", IPL_GRAY_MODE)) == NULL) {
		printf("error capturing frame\n");
		return 1;
	}
	accarr = hough(frame);
	//qsort(accarr, frame->width * frame->height * maxrad, sizeof(double), compar);

	for (r = 100; r < frame->height; r++)
		for (y = 0; y < frame->height; y++)
			for (x = 0; x < frame->width; x++)
				if (accarr[(y * frame->width + x)]) {
					//printf("x = %d y = %d r = %d\n", x, y, r);
					for (t = 0.0; t < 2 * M_PI; t += 0.01) {
						xc = x + cos(t) * r;
						yc = y + sin(t) * r;
						if (yc < frame->height && xc < frame->width && yc >= 0 && xc >= 0)
							frame->data[yc * frame->width + xc] = 0;
					}	
				}
			
	
	bzero(name, 256);
	sprintf(name, "/home/user/frame.pgm");
	//printf("%s\n", name);
	fopen(name, "w+");
	if ((fo = fopen(name, "w+")) == NULL) {
		fprintf(stderr, "error opening file: %s\n", name);
		return -1;
	}
	//fo = stdout;
	fprintf(fo, "P2\n");
	fprintf(fo, "%d %d\n", frame->width, frame->height);
	fprintf(fo, "255\n");	
	for (i = 0; i < frame->width * frame->height; i++)
		fprintf(fo, "%u\n", frame->data[i]);
	fclose(fo);
	
	ipl_freeimg(&frame);
	//ipl_freeimg(&pic);

	return 0;
}
