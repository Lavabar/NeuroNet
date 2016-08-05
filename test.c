#include <errno.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <png.h>
#include "iplimage.h"
#include "ipldefs.h"
#include <time.h>
#include "net_structs.h"
#include "netcreat.h"
#include "netpass.h"
#include "netfile.h"
#include "edge_detect.h"

#define PATH "/home/vadim/newsym"
#define NETPATH "/home/vadim/libipl/neuro.data"
#define CNTNAMES 23
#define CNT0 134
#define CNT1 68
#define CNT2 49
#define CNT3 78
#define CNT4 53
#define CNT5 61
#define CNT6 67
#define CNT7 63
#define CNT8 73
#define CNT9 63
#define CNTA 90
#define CNTB 58
#define CNTC 49
#define CNTE 61
#define CNTH 54
#define CNTK 62
#define CNTM 73
#define CNTO 62
#define CNTP 58
#define CNTT 51
#define CNTX 44
#define CNTY 47
#define CNTBAD 227
#define TOTAL CNT0 + CNT1 + CNT2 + CNT3 + CNT4 + CNT5 + CNT6 + CNT7 + CNT8 + CNTA + CNTB + CNTC + CNTE + CNTH + CNTK + CNTM + CNTO + CNTP + CNTT + CNTX + CNTY + CNTBAD
#define SAMPLE_SIZE 140

int imgcount[] =  {CNT0, CNT1, CNT2, CNT3, CNT4, CNT5, CNT6, CNT7, CNT8, CNT9, CNTA, CNTB, CNTC, CNTE, CNTH, CNTK, CNTM, CNTO, CNTP, CNTT, CNTX, CNTY, CNTBAD};
char *names[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "e", "h", "k", "m", "o", "p", "t", "x", "y", "bad"};

struct example {
	double *data;
	double *target;
};

int nn[] = {25, CNTNAMES};
int nw = SAMPLE_SIZE;
int nl = 2;
struct example *examples;

static void shufflearr(int *pathidx, int len)
{
	int p1;
	for (p1 = 0; p1 < len; ++p1) {
		int p2;
		int tmp;
		
		p2 = rand() % len;
		tmp = pathidx[p1];

		pathidx[p1] = pathidx[p2];
		pathidx[p2] = tmp;
	}
}

void train()
{
	double eta = 0.5/*, minerr = INFINITY*/;
	struct neuronet net;
	int i, j;
	double **out, minerr = DBL_MAX;
	int *idxes;
	int correct[CNTNAMES], incorrect[CNTNAMES];

	idxes = (int *)malloc(sizeof(int *) * (TOTAL));	
    net = netcreat(nl, nn, nw); 

	for (i = 0; i < (TOTAL); i++)
		idxes[i] = i;
	//first output for face, second for nonface; 	
	do {
		double newerr = 0.0;
		for (i = 0; i < CNTNAMES; i++) 
			incorrect[i] = correct[i] = 0;
			
		for (i = 0; i < (TOTAL); i++) {
			int idx, maxid;
			double maxval;

			idx = idxes[i];
			out = netfpass(&net, examples[idx].data);
			netbpass(&net, out, examples[i].target, eta);
		
			maxid = 0;
			maxval = out[net.nl - 1][0];
			for (j = 1; j < CNTNAMES; j++)
				if (out[net.nl - 1][j] > maxval || (fabs(out[net.nl - 1][j] - examples[i].target[j] < DBL_EPSILON) && examples[i].target[j] == 1.0)) {
					maxval = out[net.nl - 1][j];
					maxid = j;
				}

			if (examples[i].target[maxid] == 1.0)
				correct[maxid]++;
			else
				incorrect[maxid]++;
			
			for (j = 0; j < nl; j++)
				free(out[j]);
			free(out);
		}

		for (i = 0; i < CNTNAMES; i++) {
			printf("%s=%d/%d ", names[i], correct[i], incorrect[i]);
			newerr += (double)incorrect[i];
		}
		newerr /= (double)TOTAL;
		printf("err=%lf\n", newerr);
		if (newerr < minerr) {
			nettofile(&net, NETPATH);
			minerr = newerr;
		}
		shufflearr(idxes, (TOTAL));
	} while(1);
}
int main()
{
	char name[256];
	struct IplImage *syms[CNTNAMES][TOTAL];
	int count = 0;
	int i, j, k;

	examples = (struct example *)malloc(sizeof(struct example) * (TOTAL));
	
	for (i = 0; i < CNTNAMES; i++)
		for (j = 0; j < imgcount[i]; j++) {
			bzero(name, 256);
			sprintf(name, "%s/%s/%d.png", PATH, names[i], j);
			
			if ((syms[i][j] = ipl_readimg(name, IPL_GRAY_MODE))== NULL) {
				printf("faces: error on %s: %s\n", name, strerror(errno));
				exit(1);
			}
		}

	for (i = 0; i < CNTNAMES; i++) {
		for (j = 0; j < imgcount[i]; j++) {
			examples[count].target = (double *)calloc(nn[nl - 1], sizeof(double));
			examples[count].data = (double *)malloc(sizeof(double) * syms[i][j]->width * syms[i][j]->height);
			examples[count].target[i] = 1.0;
			for (k = 0; k < syms[i][j]->width * syms[i][j]->height; k++) {
				examples[count].data[k] = syms[i][j]->data[k] / 255.0 * 2.0 - 1.0;
			}
			count++;
		}
	}
	

	printf("reading done\n");
	train();
	printf("train done\n");
	getchar();	
	return 0;
}
