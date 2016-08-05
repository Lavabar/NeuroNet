#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include "iplimage.h"

void ipl_freeimg(struct IplImage **img)
{
	free((*img)->data);
	free(*img);
}

struct IplImage *ipl_creatimg(int w, int h, int mode)
{
	struct IplImage *res;

	res = (struct IplImage *)malloc(sizeof(struct IplImage));
	res->width = w;
	res->height = h;

	if (mode)
		res->nchans = 3;
	else
		res->nchans = 1;
	
	res->data = (unsigned char *)calloc(w * h * res->nchans, sizeof(unsigned char));
	return res;
}

struct IplImage *ipl_readimg(char *path, int mode)
{
	FILE *f;
	unsigned char head[8];
	int numb = 8, w, h, nchans = 3, y; 
	size_t sznumb;
	struct IplImage *res;
	png_bytep* rows;
	png_structp pngp;
	png_infop infop;
	
	if (!(f = fopen(path, "rb"))) { 
		perror("error opening file\n");
		return NULL;
	}

	sznumb = fread(head, sizeof(char), numb, f);
	
	if (png_sig_cmp(head, 0, 8)) {
		perror("is not png");
		fclose(f);
		return NULL;
	}
	
	if (!(pngp = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))) {
		perror("can't create read struct");
		fclose(f);
		return NULL;
	}
	
	if (!(infop = png_create_info_struct(pngp))) {
		perror("can't create info");
		png_destroy_read_struct(&pngp, NULL, NULL);
		fclose(f);
		return NULL;
	}

	if (setjmp(png_jmpbuf(pngp))) {
		perror("error in jumpbuf");
		png_destroy_read_struct(&pngp, &infop, NULL);
		fclose(f);
		return NULL;
	}

	png_init_io(pngp, f);
	png_set_sig_bytes(pngp, sznumb);

	png_read_info(pngp, infop);
	
	png_set_strip_alpha(pngp);	
	png_set_strip_16(pngp);
	if (mode) {
		nchans = 3;
		png_set_palette_to_rgb(pngp);
	} else {	
		nchans = 1;
		png_set_rgb_to_gray(pngp, 1, 0.0f, 0.0f);
		png_set_expand_gray_1_2_4_to_8(pngp);
	}
	
	png_set_interlace_handling(pngp);
	png_read_update_info(pngp, infop);

	w = png_get_image_width(pngp, infop);
	h = png_get_image_height(pngp, infop);

	rows = (png_bytep *)malloc(sizeof(png_bytep) * h);
	for (y = 0; y < h; y++) {
		rows[y] = (png_byte *)malloc(png_get_rowbytes(pngp, infop));
	}

	png_read_image(pngp, rows);

	
	
	res = (struct IplImage *)malloc(sizeof(struct IplImage));
	res->width = w;
	res->height = h;
	res->nchans = nchans;
	res->data = (unsigned char *)malloc(sizeof(unsigned char) * w * h * 3);

	for (y = 0; y < h; y++) 
		memcpy(&(res->data[nchans * y * w]), rows[y], sizeof(png_byte) * png_get_rowbytes(pngp, infop));
 
	for (y = 0; y < h; y++)
		free(rows[y]); 
	
	free(rows);

	fclose(f);
	return res;
}
