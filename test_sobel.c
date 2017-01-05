#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "edge_detect.h"
#include "iplimage.h"
#include "ipldefs.h"

int main(void)
{
	char name[256];
	struct IplImage *img;
	int i, k = 0;
		
	bzero(name, 256);
	sprintf(name, "/home/user/img.png");
	if ((img = ipl_readimg(name, IPL_RGB_MODE)) == NULL) {
		printf("error reding image\n");
		return 1;
	}
	data = sobel(img, 100);
		
	char name1[256];
	FILE *f;
	int y;
	sprintf(name, "/home/user/imgimg.pgm");
	f = fopen(name, "w+");	
	fprintf(f, "P2\n");
	fprintf(f, "28 28\n");
	fprintf(f, "1\n");
	for (y = 0; y < 28 * 28; y++)
		fprintf(f, "%d\n", (int)data[y]);
	fclose(f);


	return 0;
}
