#ifndef IPL_TRANSFORM_H
#define IPL_TRANSFORM_H

#include "vecmat.h"
#include "iplimage.h"

struct IplImage *transformsur(struct IplImage *insur, struct matrix3 *m);
struct IplImage *ipl_scaleimg(struct IplImage *insur, int neww, int newh);

#endif
