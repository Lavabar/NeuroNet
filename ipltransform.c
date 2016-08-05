#include <stdlib.h>
#include <math.h>

#include "ipltransform.h"

struct IplImage *transformsur(struct IplImage *insur, struct matrix3 *m)
{
	int inw;
	int inh;
	int outw;
	int outh;
	int outx;
	int outy;
	unsigned char *outdata;
	unsigned char *indata;
	struct matrix3 im;

	struct IplImage *outsur;

	m3inverse(m, &im);
	
	indata = insur->data;
	inw = insur->width;
	inh = insur->height;

	outw = inw * (double) m->_11;	
	outh = inh * (double) m->_22;

	outsur = ipl_creatimg(outw, outh, insur->nchans - 1);

	outdata = outsur->data;
	for (outy = 0; outy < outh; ++outy)
		for (outx = 0; outx < outw; ++outx) {
			int inx0, iny0;
			int inx1, iny1;
			struct vector2 inp;
			struct vector2 outp;
			double d00, d01, d10, d11;
			double sumd;

			int c;

			outp.x = (double) outx;
			outp.y = (double) outy;

			v2m3mult(&outp, &im, &inp);

			inx0 = (int) inp.x;
			iny0 = (int) inp.y;

			inx1 = (int) inp.x + 1;
			iny1 = (int) inp.y + 1;

			d00 = sqrt(pow((double) iny0 - inp.y, 2.0) 
				+ pow((double) inx0 - inp.x, 2.0));

			d01 = sqrt(pow((double) iny0 - inp.y, 2.0) 
				+ pow((double) inx1 - inp.x, 2.0));

			d10 = sqrt(pow((double) iny1 - inp.y, 2.0) 
				+ pow((double) inx0 - inp.x, 2.0));

			d11 = sqrt(pow((double) iny1 - inp.y, 2.0) 
				+ pow((double) inx1 - inp.x, 2.0));

			if (iny0 < 0)
				d00 = d01 = 0.0;
			if (iny1 >= inh)
				d10 = d11 = 0.0;
			if (inx0 < 0)
				d00 = d10 = 0.0;
			if (inx1 >= inw)
				d01 = d11 = 0.0;

			sumd = d00 + d01 + d10 + d11;

			d00 = sumd - d00;
			d01 = sumd - d01;
			d10 = sumd - d10;
			d11 = sumd - d11;

			sumd = d00 + d01 + d10 + d11;
			
			d00 /= sumd;
			d01 /= sumd;
			d10 /= sumd;
			d11 /= sumd;

			for (c = 0; c < insur->nchans; c++) {
				unsigned char c00, c01, c10, c11;
	
				if ((iny0 >= 0 && iny0 < inh) && (inx0 >= 0 && inx0 < inw))
					c00 = indata[insur->nchans * (iny0 * inw + inx0) + c];
				else
					c00 = 0;

				if ((iny0 >= 0 && iny0 < inh) && (inx1 >= 0 && inx1 < inw))
					c01 = indata[insur->nchans * (iny0 * inw + inx1) + c];
				else
					c01 = 0;

				if ((iny1 >= 0 && iny1 < inh) && (inx0 >= 0 && inx0 < inw))
					c10 = indata[insur->nchans * (iny1 * inw + inx0) + c];
				else
					c10 = 0;
	
				if ((iny1 >= 0 && iny1 < inh) && (inx1 >= 0 && inx1 < inw))
					c11 = indata[insur->nchans * (iny1 * inw + inx1) + c];
				else
					c11 = 0;

				outdata[outsur->nchans * (outy * outw + outx) + c]
					= d00 * c00 + d01 * c01
						+ d10 * c10 + d11 * c11;
			}
	}

	return outsur;
}


struct IplImage *ipl_scaleimg(struct IplImage *insur, int neww, int newh)
{
	struct matrix3 m;

	m3scale(&m, (double) neww / insur->width, (double) newh / insur->height);
	
	return transformsur(insur, &m);
}
