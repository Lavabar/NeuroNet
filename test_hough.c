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

	struct IplImage *frame, *pic;
	//struct IplDev *dev1;
	FILE *fo;
	int i;
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

	pic = sobel(frame);
	
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
	fprintf(fo, "%d %d\n", pic->width, pic->height);
	fprintf(fo, "255\n");	
	for (i = 0; i < pic->width * pic->height * pic->nchans; i++)
		fprintf(fo, "%u\n", pic->data[i]);
	fclose(fo);

	ipl_freeimg(&frame);
	ipl_freeimg(&pic);

	return 0;
}
