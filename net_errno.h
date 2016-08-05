#ifndef NET_ERRNO_H
#define NET_ERRNO_H

enum NET_ERRNO {
	NET_SUCCESS,
	NET_ENOMEM,
	NET_EINVAL
};

extern enum NET_ERRNO net_errno;

#endif //NET_ERRNO_H
