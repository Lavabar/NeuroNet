#include <stdio.h>
#include <stdlib.h>
#include "edge_detect.h"
#include "ipltransform.h"
#include "iplimage.h"

int main(void)
{
	char name[256];
	struct IplImage img;
	int i, k = 0;
	
	for (i = 0; i < 15; i++) {
		bzero(name, 256);
		sprintf(name, "/home/user/NeuroNet/guns/%d.png", i);
		if ((img = ipl_readimg(name, IPL_RGB_MODE)) == NULL) {
			printf("error reding image\n");
			return 1;
		}
	
	
		sobel(&img, 250);	

	}


	return 0;
}
