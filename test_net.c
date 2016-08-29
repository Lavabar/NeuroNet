#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <string.h>
#include <png.h>
#include "iplimage.h"
#include "iplvideo.h"
#include "ipldefs.h"

#include "neurowork.h"
#include "net_structs.h"
#include "netfile.h"
#include "net_def.h"

#define GUN_NEURO_PATH "/home/user/NeuroNet/gun_neuro.data"
#define NOTGUN_NEURO_PATH "/home/user/NeuroNet/notgun_neuro.data"

int main()
{
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

	struct IplImage *frame;
	struct IplDev *dev1;
	FILE *fo;
	int f, i;
	char name[256];

	
	f = 0;
	if ((dev1 = ipl_opendev(0, IPL_RGB_MODE)) == NULL)
		printf("error while creating device 0\n");
	if (ipl_setparams(dev1, 320, 240, IPL_FORCE_SCALE_OFF) < 0) {
		fprintf(stderr, "error on changing cam params\n");
		free(dev1);
	}

	while (1) {
		f++;	
	
		if ((frame = ipl_getframe(dev1)) == NULL) {
			printf("error capturing curr1\n");
		}
		ipl_scaleimg(&frame, 640, 480);

		if(neurowork(frame))
			fprintf(stderr, "error in neurowork\n");

		bzero(name, 256);
		sprintf(name, "/home/user/frame%d.ppm", f);
		fo = fopen(name, "w+");
		fprintf(fo, "P3\n");
		fprintf(fo, "%d %d\n", frame->width, frame->height);
		fprintf(fo, "255\n");
		

		for (i = 0; i < frame->width * frame->height * frame->nchans; i++)
			fprintf(fo, "%u\n", frame->data[i]);
		fclose(fo);

		ipl_freeimg(&frame);
	}

	return 0;
}
