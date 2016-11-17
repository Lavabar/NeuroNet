#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net_structs.h"
#include "net_errno.h"
#include "net_def.h"
#include "netfile.h"

#define GUN_NEURO_PATH "/home/user/NeuroNet/gun_neuro.data"
#define NOTGUN_NEURO_PATH "/home/user/NeuroNet/notgun_neuro.data"

void net_count()
{
	int i, j;
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
	double *mas;
	double *w;
	mas = (double *)malloc(sizeof(double) * 32 * gun_net->total_nw);
	w = gun_net->w;
	for (i = 0; i < gun_net->total_nw; i++)
		for (j = 0; j < 32; j++)
			mas[i * 32 + j] = j * *(w++);
	free(mas);
	mas = (double *)malloc(sizeof(double) * 32 * notgun_net->total_nw);
	w = notgun_net->w;
	for (i = 0; i < notgun_net->total_nw; i++)
		for (j = 0; j < 32; j++)
			mas[i * 32 + j] = j * *(w++);
	free(mas);


}
