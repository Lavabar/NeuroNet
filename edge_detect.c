#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <png.h>
#include "iplimage.h"
#include "ipldefs.h"

static int corrind(int x, int y, int w, int h)
{
	int x1, y1;

	x1 = (x < 0) ? 0 : x;
	x1 = (x1 >= w)? w - 1 : x1;


	y1 = (y < 0) ? 0 : y;
	y1 = (y1 >= h)? h - 1 : y1;

	return y1 * w + x1;
}

static void grad(const unsigned char *vdata, int w, int h, int x, int y, double *gx, double *gy)
{
/*	*gx = vdata[corrind(x - 1, y + 1, w, h)] + 2.0 * vdata[corrind(x, y + 1, w, h)] + vdata[corrind(x + 1, y + 1, w, h)]
		- (vdata[corrind(x - 1, y - 1, w, h)] + 2.0 * vdata[corrind(x, y - 1, w, h)] + vdata[corrind(x + 1, y - 1, w, h)]);
	*gy = vdata[corrind(x + 1, y - 1, w, h)] + 2.0 * vdata[corrind(x + 1, y, w, h)] + vdata[corrind(x + 1, y + 1, w, h)]
		- (vdata[corrind(x - 1, y - 1, w, h)] + 2.0 * vdata[corrind(x - 1, y, w, h)] + vdata[corrind(x - 1, y + 1, w, h)]);*/

	*gx = vdata[corrind(x - 1, y + 1, w, h)] + 4.0 * vdata[corrind(x, y + 1, w, h)] + vdata[corrind(x + 1, y + 1, w, h)]
		- (vdata[corrind(x - 1, y - 1, w, h)] + 4.0 * vdata[corrind(x, y - 1, w, h)] + vdata[corrind(x + 1, y - 1, w, h)]);
	*gy = vdata[corrind(x + 1, y - 1, w, h)] + 4.0 * vdata[corrind(x + 1, y, w, h)] + vdata[corrind(x + 1, y + 1, w, h)]
		- (vdata[corrind(x - 1, y - 1, w, h)] + 4.0 * vdata[corrind(x - 1, y, w, h)] + vdata[corrind(x - 1, y + 1, w, h)]);

}
double *sobel(struct IplImage *frame, double thres)
{
	int x, y, w, h;
	double *res;

	h = frame->height;
	w = frame->width;
	res = (double *)malloc(sizeof(double) * w * h);
  
	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++)
			res[y * w + x] = -1.0;
  
	for(y = 1; y < h - 1; y++){
		for(x = 1; x < w - 1; x++) {
			double gx, gy, gr;
			
			grad(frame->data, w, h, x, y, &gx, &gy);

			gr = sqrt(gx * gx + gy * gy);
      
			if (gr >= thres) {
				res[y * w + x] = 1.0;
			}   
			
		}
	}

	return res;
}


