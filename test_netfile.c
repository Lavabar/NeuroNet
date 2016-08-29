#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "draw.h"

#include <string.h>
#include <png.h>
#include "iplimage.h"
#include "iplvideo.h"
#include "ipldefs.h"

#include "net_structs.h"
#include "netfile.h"
#include "netpass.h"
#include "net_errno.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define SAMPLE_HEIGHT 20
#define SAMPLE_WIDTH 50
#define SCALE_RATE 0.7

#define NEURO_PATH "/home/user/NeuroNet/neuro.data"

static void activate(GtkApplication* app, gpointer user_data)
{
	printf("12");
}


int main(int argc, char **argv)
{
	GtkApplication *app;
  	int status;

	struct neuronet *net = malloc(sizeof(struct neuronet));

	netfromfile(net, NEURO_PATH);
	
	app = gtk_application_new("org.gtk", G_APPLICATION_FLAGS_NONE);
  	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  	status = g_application_run(G_APPLICATION(app), argc, argv);
  	g_object_unref(app);

  	return status;
}
