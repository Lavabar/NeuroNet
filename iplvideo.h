#ifndef _H_IPLVIDEO
#define _H_IPLVIDEO

struct IplDev {
	int fd, width, height, fmt, mode, iometh;
	unsigned int length, offset;
};

struct IplDev *ipl_opendev(int id, int mode);
struct IplImage *ipl_getframe(struct IplDev *dev);
#endif
