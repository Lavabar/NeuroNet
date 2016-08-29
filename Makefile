CC=gcc
NETSRCS=netcreat.c net_errno.c netpass.c netfile.c edge_detect.c iplimage.c draw.c iplvideo.c neurowork.c
CFLAGS=-Wall -g `pkg-config --libs gtk+-3.0` -lm -lpng12 `pkg-config --cflags gtk+-3.0` 
NETOBJS=$(NETSRCS:.c=.o)

all: test_netcreat test_fpass test_netfile test_main guntrain notguntrain beta_main main test_net #bigmain

test_fpass: $(NETOBJS) test_fpass.o
	$(CC) $(CFLAGS) -o test_fpass test_fpass.o $(NETOBJS)

test_netcreat: $(NETOBJS) test_netcreat.o
	$(CC) $(CFLAGS) -o test_netcreat test_netcreat.o $(NETOBJS)

test_netfile: $(NETOBJS) test_netfile.o
	$(CC) $(CFLAGS) -o test_netfile test_netfile.o $(NETOBJS)

test_main: $(NETOBJS) test_main.o
	$(CC) $(CFLAGS) -o test_main test_main.o $(NETOBJS)

guntrain: $(NETOBJS) guntrain.o
	$(CC) $(CFLAGS) -o guntrain guntrain.o $(NETOBJS)

notguntrain: $(NETOBJS) notguntrain.o
	$(CC) $(CFLAGS) -o notguntrain notguntrain.o $(NETOBJS)

beta_main: $(NETOBJS) beta_main.o
	$(CC) $(CFLAGS) -o beta_main beta_main.o $(NETOBJS)

main: $(NETOBJS) main.o
	$(CC) $(CFLAGS) -o main main.o $(NETOBJS)

#bigmain: $(NETOBJS) bigmain.o
#	$(CC) $(CFLAGS) -o bigmain bigmain.o $(NETOBJS)

test_net: $(NETOBJS) test_net.o
	$(CC) $(CFLAGS) -o test_net test_net.o $(NETOBJS)

test_fpass.o:test_fpass.c
	$(CC) $(CFLAGS) -c test_fpass.c	

test_netcreat.o:test_netcreat.c
	$(CC) $(CFLAGS) -c test_netcreat.c
	
test_netfile.o:test_netfile.c
	$(CC) $(CFLAGS) -c test_netfile.c	

test_main.o:test_main.c
	$(CC) $(CFLAGS) -c test_main.c

guntrain.o:guntrain.c
	$(CC) $(CFLAGS) -c guntrain.c

notguntrain.o:notguntrain.c
	$(CC) $(CFLAGS) -c notguntrain.c

beta_main.o:beta_main.c
	$(CC) $(CFLAGS) -c beta_main.c

main.o:main.c
	$(CC) $(CFLAGS) -c main.c

#bigmain.o:bigmain.c
#	$(CC) $(CFLAGS) -c bigmain.c

test_net.o:test_net.c
	$(CC) $(CFLAGS) -c test_net.c

neurowork.o: neurowork.c
	$(CC) $(CFLAGS) -c neurowork.c

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

iplvideo.o: iplvideo.c
	$(CC) $(CFLAGS) -c iplvideo.c

draw.o: draw.c
	$(CC) $(CFLAGS) -c draw.c

clean:
	rm test_netcreat test_fpass test_netfile test_main train beta_main main test_net *.o #bigmain
