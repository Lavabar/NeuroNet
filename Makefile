CC=gcc
#SRCS=iplimage.c iplvideo.c netcreat.c netpass.c netfile.c ipltransform.c vecmat.c edge_detect.c
NETSRCS=netcreat.c net_errno.c netpass.c netfile.c edge_detect.c iplimage.c
CFLAGS=-Wall -g -lm -lpng12
NETOBJS=$(NETSRCS:.c=.o)

#all: $(SRCS) test.c test1.c test2.c test3.c
#	$(CC) $(CFLAGS) -I/usr/include/libpng12 -lpng12 -o test test.c $(SRCS)
#	$(CC) $(CFLAGS) -I/usr/include/libpng12 -lpng12 -o test1 test1.c $(SRCS)
#	$(CC) $(CFLAGS) -I/usr/include/libpng12 -lpng12 -o test2 test2.c $(SRCS)
#	$(CC) $(CFLAGS) -I/usr/include/libpng12 -lpng12 -o test3 test3.c $(SRCS)
#	$(CC) $(CFLAGS) -I/usr/include/libpng12 -lpng12 -o testback testback.c $(SRCS)

all: test_netcreat test_fpass test_netfile test_main train

test_fpass: $(NETOBJS) test_fpass.o
	$(CC) $(CFLAGS) -o test_fpass test_fpass.o $(NETOBJS)

test_netcreat: $(NETOBJS) test_netcreat.o
	$(CC) $(CFLAGS) -o test_netcreat test_netcreat.o $(NETOBJS)

test_netfile: $(NETOBJS) test_netfile.o
	$(CC) $(CFLAGS) -o test_netfile test_netfile.o $(NETOBJS)

test_main: $(NETOBJS) test_main.o
	$(CC) $(CFLAGS) -o test_main test_main.o $(NETOBJS)

train: $(NETOBJS) train.o
	$(CC) $(CFLAGS) -o train train.o $(NETOBJS)

test_fpass.o:test_fpass.c
	$(CC) $(CFLAGS) -c test_fpass.c	

test_netcreat.o:test_netcreat.c
	$(CC) $(CFLAGS) -c test_netcreat.c
	
test_netfile.o:test_netfile.c
	$(CC) $(CFLAGS) -c test_netfile.c	

test_main.o:test_main.c
	$(CC) $(CFLAGS) -c test_main.c

train.o:train.c
	$(CC) $(CFLAGS) -c train.c

netpass.o: netpass.c
	$(CC) $(CFLAGS) -c netpass.c

netcreat.o: netcreat.c
	$(CC) $(CFLAGS) -c netcreat.c

netfile.o: netfile.c
	$(CC) $(CFLAGS) -c netfile.c

net_errno.o: net_errno.c
	$(CC) $(CFLAGS) -c net_errno.c

edge_detect.o: edge_detect.c
	$(CC) $(CFLAGS) -c edge_detect.c

iplimage.o: iplimage.c
	$(CC) $(CFLAGS) -c iplimage.c

clean:
	rm test_netcreat test_fpass test_netfile test_main train *.o



