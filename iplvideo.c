#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include "iplimage.h"
#include "iplvideo.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

enum iometh {
	IO_READ,
	IO_MMAP
};

static int xioctl(int fd, int request, void *arg)
{
	int r;

	do {
		r = ioctl(fd, request, arg);
	} while (r == -1 && errno == EINTR);

	return r;
}

static unsigned char *ioread(struct IplDev *dev)
{
	unsigned char *res;
 
	res = (unsigned char *)malloc(dev->length);
//	printf("size=%zd\n", dev->length);
	if (read(dev->fd, res, dev->length) == -1) {
		free(res); 
		return NULL;
	}
	return res;	
}

static int startcap(struct IplDev *dev)
{
	struct v4l2_buffer buf;
	enum v4l2_buf_type type;

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;
	
	if (xioctl(dev->fd, VIDIOC_QBUF, &buf) == -1)
		return -1;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (xioctl(dev->fd, VIDIOC_STREAMON, &type) == -1)
		return -1;

	return 1;
}

static int readframe(struct IplDev *dev)
{
	struct v4l2_buffer buf;

	CLEAR(buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	
	if (xioctl(dev->fd, VIDIOC_DQBUF, &buf) == -1) {
		if (errno == EAGAIN)
			return 0;
		return -1;
	}
	
//	printf("used=%d length=%d\n", buf.bytesused, dev->length);
	if (xioctl(dev->fd, VIDIOC_QBUF, &buf) == -1)
		return -1;
	
	return 1;
}

static unsigned char *iommap(struct IplDev *dev)
{
	int rc;
	unsigned char *res;
	
	res = NULL;
	res = mmap(NULL, dev->length, PROT_READ | PROT_WRITE, MAP_SHARED, dev->fd, dev->offset);
	
	if (res == MAP_FAILED) 
		return NULL;
	
	while ((rc = readframe(dev)) == 0);
	
	if (rc < 0) {
		free(res);
		fprintf(stderr, "could not read frame\n");
		return NULL;
	}
	
	return res;
}
/*#define R_FROM_YUV(y, u, v) (int)((double)y + 1.13983 * ((double)v - 128.0))
#define G_FROM_YUV(y, u, v) (int)((double)y - 0.39465 * ((double)u - 128.0) - 0.58060 * ((double)v - 128.0))
#define B_FROM_YUV(y, u, v) (int)((double)y + 2.03211 * ((double)u - 128.0))*/


#define R_FROM_YUV(y, u, v) (int)((double)y + 1.13983 * ((double)v - 128.0))
#define G_FROM_YUV(y, u, v) (int)((double)y - 0.39465 * ((double)u - 128.0) - 0.58060 * ((double)v - 128.0))
#define B_FROM_YUV(y, u, v) (int)((double)y + 2.03211 * ((double)u - 128.0))

static struct IplImage *yuv422_to_rgb(struct IplDev *dev, unsigned char *data) 
{
	int i, j;
	struct IplImage *res;
	
	res = malloc(sizeof(struct IplImage));
	res->width = dev->width;
	res->height = dev->height;
	res->nchans = 3;
	res->data = (unsigned char *)malloc(sizeof(unsigned char) * res->width * res->height * 3);
	for (i = 0, j = 0; i < dev->length; i += 4, j += 6) {
		unsigned char y1, y2, u, v;
		unsigned char r1, g1, b1, r2, g2, b2;
		
		y1 = data[i];
		u = data[i + 1];
		y2 = data[i + 2];
		v = data[i + 3];

		r1 = R_FROM_YUV(y1, u, v);
		g1 = G_FROM_YUV(y1, u, v);
		b1 = B_FROM_YUV(y1, u, v);

		r2 = R_FROM_YUV(y2, u, v);
		g2 = G_FROM_YUV(y2, u, v);
		b2 = B_FROM_YUV(y2, u, v);

		res->data[j] = r1;
		res->data[j + 1] = g1;
		res->data[j + 2] = b1;

		res->data[j + 3] = r2;
		res->data[j + 4] = g2;
		res->data[j + 5] = b2;
	}
	return res; 
}

static struct IplImage *yuv422_to_gray(struct IplDev *dev, unsigned char *data) 
{
	int i, j;
	struct IplImage *res;
	
	res = malloc(sizeof(struct IplImage));
	res->width = dev->width;
	res->height = dev->height;
	res->nchans = 1;
	res->data = (unsigned char *)malloc(sizeof(unsigned char) * res->width * res->height);
	for (i = 0, j = 0; i < dev->length; i += 4, j += 2) {
		unsigned char y1, y2;
		
		y1 = data[i];
		y2 = data[i + 2];

		res->data[j] = y1;
		res->data[j + 1] = y2;
	}
	return res; 
}

struct IplImage *ipl_getframe(struct IplDev *dev)
{
	unsigned char *data;
	
	if (dev->iometh == IO_READ) 
		data = ioread(dev);
	else if (dev->iometh == IO_MMAP) 
		data = iommap(dev);
	else 
		return NULL;
	
	if (!data)
		return NULL;

	if (dev->mode)
		return yuv422_to_rgb(dev, data);
	else 
		return yuv422_to_gray(dev, data);
}

static int setiometh(struct IplDev *dev, struct v4l2_capability *cap, char *devname)
{	
	if (!(cap->capabilities & V4L2_CAP_READWRITE)) {
		if (errno == EINVAL)
			fprintf(stderr, "%s does not support read i/o\n", devname);
	} else { 
		dev->iometh = IO_READ;
		return 1;
	}
	
	if (!(cap->capabilities & V4L2_CAP_STREAMING))
		fprintf(stderr, "%s does not support streaming\n", devname);
	else { 
		dev->iometh = IO_MMAP;
		struct v4l2_requestbuffers req;
		struct v4l2_buffer buf;

		CLEAR(req);
		req.count = 1;
		req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory = V4L2_MEMORY_MMAP;

		if (xioctl(dev->fd, VIDIOC_REQBUFS, &req)) {
			if (errno == EINVAL)
				fprintf(stderr, "does not support memory mapping\n");
			return -1;
		}
	
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = 0;
		if (xioctl(dev->fd, VIDIOC_QUERYBUF, &buf) == -1) {
			fprintf(stderr, "could not query buffer\n");
			return -1;
		}
	
	
		if (startcap(dev) < 0) {
			fprintf(stderr, "already started capturing\n");
			return -1;
		}

		dev->length = buf.length;
		dev->offset = buf.m.offset;

		return 1;	
	}
	return -1;
}

struct IplDev *ipl_opendev(int id, int mode)
{ 
	struct stat st;
	int fd;
	struct IplDev *dev;
	char devname[64];
	struct v4l2_capability cap;
	struct v4l2_format fmt;

	bzero(devname, 64);
	sprintf(devname, "/dev/video%d", id);

	if (stat(devname, &st) == -1) {
		fprintf(stderr, "can not identify %s\n", devname);
		return NULL;
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", devname);
		return NULL;
	}
	
	if ((fd = open(devname, O_RDWR | O_NONBLOCK, 0)) == -1) {
		fprintf(stderr, "can not open %s\n", devname);
		return NULL;
	}
	
	dev = (struct IplDev *)malloc(sizeof(struct IplDev));
	dev->fd = fd;
	dev->mode = mode;

	if (xioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) { 
		fprintf(stderr, "%s is not V4L2 device\n", devname);
		free(dev);
		close(fd);
		return NULL;
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		if (errno == EINVAL)
			fprintf(stderr, "not video device\n");
		free(dev);
		close(fd);
		return NULL;
	}	

	if (setiometh(dev, &cap, devname) < 0) {
		fprintf(stderr, "can not set io method\n");
		close(fd);
		free(dev);
		return NULL;
	}

	CLEAR(fmt);	
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	
	if (xioctl(fd, VIDIOC_G_FMT, &fmt) == -1) {
		fprintf(stderr, "error on getting pix format\n");
		free(dev);
		close(fd);
		return NULL;
	}
	
	dev->width = fmt.fmt.pix.width;
	dev->height = fmt.fmt.pix.height;
	dev->fmt = fmt.fmt.pix.pixelformat;
	return dev;
}
