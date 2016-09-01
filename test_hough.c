#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <string.h>
#include <png.h>
#include "iplimage.h"
#include "iplvideo.h"
#include "ipldefs.h"
#include "hough.h"

int main()
{

	struct IplImage *frame;
	//struct IplDev *dev1;
	FILE *fo;
	int thres, i;
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

	thres = otsu(frame);
	printf("thres = %d\n", thres);
	for (i = 0; i < frame->width * frame->height; i++) {
		if (frame->data[i] >= thres)
			frame->data[i] = 255;
		else
			frame->data[i] = 0;
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
	for (i = 0; i < frame->width * frame->height * frame->nchans; i++)
		fprintf(fo, "%u\n", frame->data[i]);
	fclose(fo);

	ipl_freeimg(&frame);

	return 0;
}
