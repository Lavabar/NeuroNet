#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "iplimage.h"
#include "ipldefs.h"

#define N_NEW_CLR 256
#define MEASURE_ERROR (N_NEW_CLR/4)

/*unsigned char otsu(struct IplImage *img)
{
	unsigned char min, max, thres;
	int *hist;
	int i;
	double w1, w2, u1, u2, ut, tmp, sigma, maxsigma;

	int square = (img->width * img->height);

	maxsigma = -1;
	tmp = 0;
	min = 255;
	max = 0;
	for (i = 0; i < square; i++) {
		if (img->data[i] >= max)
			max = img->data[i];
		if (img->data[i] <= min)
			min = img->data[i];
	}
	if((hist = (int *)calloc((max - min + 1), sizeof(int))) == NULL) {
		fprintf(stderr, "error in alloc hist");
		return -1;
	}

	for (i = 0; i <= square; i++)
		hist[img->data[i] - min]++;
	
	for (i = 0; i < (max - min); i++)
		tmp += hist[i] * i;
	ut = (double)tmp / square;
	int a = 0;
	int b = 0;
	for (i = 0; i < (max - min); i++) {
		a += hist[i];
		w1 = (double)a / square;
		w2 = 1 - w1;
		b += hist[i] * i;
		u1 = (double)b / (square * w1);
		u2 = (ut - u1 * w1) / w2;
		sigma = w1 * w2 * (u1 - u2) * (u1 - u2);
		if (sigma >= maxsigma) {
			maxsigma = sigma;
			thres = i;
		}
	}
	free(hist);

	return thres + min;
}*/

static unsigned char otsu(struct IplImage *img)
{
	int min=img->data[0], max=img->data[0];
	int i, temp, temp1;
	int *hist;
	int histSize;
	int size = img->width * img->height;

	int alpha, beta, threshold = 0;
	double sigma, maxSigma = -1;
	double w1,a;

/**** Построение гистограммы ****/
/* Узнаем наибольший и наименьший полутон */
	for (i = 1; i < size; i++) {
		temp = img->data[i];
		if(temp < min)min = temp;
		if(temp > max)max = temp;
	}

	histSize = max - min + 1;
	
	if((hist = (int *)malloc(sizeof(int) * histSize)) == NULL) 
		return -1;

	for (i = 0; i < histSize; i++)
		hist[i] = 0;

/* Считаем сколько каких полутонов */
	for (i = 0; i < size; i++)
		hist[img->data[i]-min]++;

/**** Гистограмма построена ****/

	temp = temp1 = 0;
	alpha = beta = 0;
/* Для расчета математического ожидания первого класса */
	for (i = 0; i <= (max - min); i++) {
		temp += i*hist[i];
		temp1 += hist[i];
	}

/* Основной цикл поиска порога
Пробегаемся по всем полутонам для поиска такого, при котором внутриклассовая дисперсия минимальна */
	for (i = 0; i < (max - min); i++) {
		alpha += i * hist[i];
		beta += hist[i];

	w1 = (double)beta / temp1;
	a = (double)alpha / beta - (double)(temp - alpha) / (temp1 - beta);
	sigma = w1 * (1 - w1) * a * a;

	if (sigma > maxSigma) {
		maxSigma=sigma;
		threshold=i;
		}
	}
	free(hist);
	return threshold + min;
}

static void quantization(struct IplImage *img)
{
	int i, noldclr, nnewclr;
	noldclr = 256;
	nnewclr = N_NEW_CLR;

	for (i = 0; i < img->width * img->height; i++)
		img->data[i] /= (noldclr / nnewclr); 
	 
}

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
	*gx = vdata[corrind(x - 1, y + 1, w, h)] + 2.0 * vdata[corrind(x, y + 1, w, h)] + vdata[corrind(x + 1, y + 1, w, h)]
		- (vdata[corrind(x - 1, y - 1, w, h)] + 2.0 * vdata[corrind(x, y - 1, w, h)] + vdata[corrind(x + 1, y - 1, w, h)]);
	*gy = vdata[corrind(x + 1, y - 1, w, h)] + 2.0 * vdata[corrind(x + 1, y, w, h)] + vdata[corrind(x + 1, y + 1, w, h)]
		- (vdata[corrind(x - 1, y - 1, w, h)] + 2.0 * vdata[corrind(x - 1, y, w, h)] + vdata[corrind(x - 1, y + 1, w, h)]);

	/* *gx = vdata[corrind(x - 1, y + 1, w, h)] + 4.0 * vdata[corrind(x, y + 1, w, h)] + vdata[corrind(x + 1, y + 1, w, h)]
		- (vdata[corrind(x - 1, y - 1, w, h)] + 4.0 * vdata[corrind(x, y - 1, w, h)] + vdata[corrind(x + 1, y - 1, w, h)]);
	*gy = vdata[corrind(x + 1, y - 1, w, h)] + 4.0 * vdata[corrind(x + 1, y, w, h)] + vdata[corrind(x + 1, y + 1, w, h)]
		- (vdata[corrind(x - 1, y - 1, w, h)] + 4.0 * vdata[corrind(x - 1, y, w, h)] + vdata[corrind(x - 1, y + 1, w, h)]);*/

}
double *hough(struct IplImage *frame)
{
	int x, y, r, w, h, maxrad;
	double diag;
	double *accarr; 
	unsigned char thres;
	//quantization(frame);
	h = frame->height;
	w = frame->width;
	diag = sqrt(w * w + h * h);
	maxrad = h;
  	thres = otsu(frame);
	accarr = malloc(sizeof(double) * w * h * maxrad);	

	bzero(accarr, w * h * maxrad);
  
	for(y = 0; y < h; y++){
		for(x = 0; x < w; x++) {
			double gx, gy, gr;
			double theta;
			int x0, y0;
			grad(frame->data, w, h, x, y, &gx, &gy);

			theta = atan2(gy, gx);
				
			gr = sqrt(gx * gx + gy * gy);

			if (gr >= (thres + MEASURE_ERROR)) {
				for (r = 100; r < maxrad; r++) {
					x0 = x - r * cos(theta);
					y0 = y - r * sin(theta);
					if (x0 >= 0 && y0 >= 0 && x0 < w && y0 < h)
						accarr[(y0 * h + x0) * r]++;
				}
			}   
			
		}
	}
	
	//double avr = 0.0;
	double max;
	max = 0;
	for(r = 10; r < maxrad; r++)
		for (y = 0; y < h; y++)
			for (x = 0; x < w; x++)
				if (accarr[(y * w + x) * r] >= max)
					max = accarr[(y * w + x) * r];
//				avr += accarr[(y * w + x) * r];
//	avr /= (w * h * (maxrad - 10));
//	printf("%lf\n", avr);

	for(r = 100; r < maxrad; r++)
		for (y = 0; y < h; y++)
			for (x = 0; x < w; x++)
				if (accarr[(y * w + x) * r] < max)
					accarr[(y * w + x) * r] = 0.0;


	return accarr;
}
