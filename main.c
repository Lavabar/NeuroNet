#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <string.h>
#include <png.h>
#include "iplimage.h"
#include "iplvideo.h"
#include "ipldefs.h"

#include "neurowork.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>

int flag;

static void print_txt(GtkWidget *widget, gpointer data)
{
  g_print ("Cancel\n");
}

static void flag_zero(GtkWidget *widget, gpointer data)
{
	flag = 0;
}

static int camera_shoot(struct IplImage *img, GdkPixbuf *pixbuf)
{
	int x, y;
	int rowstride;
  	guchar *pixels, *p;

	
	g_assert (gdk_pixbuf_get_colorspace (pixbuf) == GDK_COLORSPACE_RGB);
  	g_assert (gdk_pixbuf_get_bits_per_sample (pixbuf) == 8);
  	g_assert (!gdk_pixbuf_get_has_alpha (pixbuf));
  	g_assert (img->nchans == 3);

	for (y = 0; y < img->height; y++)
		for (x = 0; x < img->width; x++) {
			unsigned char r, g, b;
			r = img->data[img->nchans * (y * img->width + x) + 0];
			g = img->data[img->nchans * (y * img->width + x) + 1];
			b = img->data[img->nchans * (y * img->width + x) + 2];
			rowstride = gdk_pixbuf_get_rowstride (pixbuf);
  			pixels = gdk_pixbuf_get_pixels (pixbuf);

  			p = pixels + y * rowstride + x * img->nchans;
  			p[0] = r;
  			p[1] = g;
  			p[2] = b;
		}

	return 0;	
}
static void activate (GtkApplication* app, gpointer user_data)
{
	GtkWidget *window;
	GtkWidget *image;
	GtkWidget *button, *box;
	GdkPixbuf *pixbuf;

  	window = gtk_application_window_new(app);
  	gtk_window_set_title(GTK_WINDOW(window), "Window");
  	gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
	
	image = gtk_image_new();
	pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, 640, 480);
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	button = gtk_button_new_with_label ("Cancel");
  	g_signal_connect(button, "clicked", G_CALLBACK(print_txt), NULL);
  	g_signal_connect_swapped(button, "clicked", G_CALLBACK(flag_zero), window);

  	gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
  	gtk_container_add(GTK_CONTAINER(window), box);
	
	struct IplImage *frame;
	struct IplDev *dev1;

	time_t start, end;
	int f;

	
	f = 0;
	start = 0;
	end = 0;
	if ((dev1 = ipl_opendev(0, IPL_RGB_MODE)) == NULL)
		printf("error while creating device 0\n");
	if (ipl_setparams(dev1, 320, 240, IPL_FORCE_SCALE_OFF) < 0) {
		fprintf(stderr, "error on changing cam params\n");
		free(dev1);
	}

	flag = 1;
	start = time(NULL);
	
	while (flag) {
		f++;	
		end = time(NULL);
		if (end - start >= 1.0) {
			printf("fps = %i\n", f);
			f = 0;
			start = end;
		}
	
		if ((frame = ipl_getframe(dev1)) == NULL) {
			printf("error capturing curr1\n");
		}
		ipl_scaleimg(&frame, 640, 480);

		if(neurowork(frame))
			fprintf(stderr, "error in neurowork\n");

		camera_shoot(frame, pixbuf);
		
		gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
		gtk_widget_queue_draw(image);

  		gtk_widget_show_all (window);
		gtk_main_iteration_do(FALSE);

		ipl_freeimg(&frame);
	}
}

int main(int argc, char **argv)
{
	GtkApplication *app;
  	int status;

  	app = gtk_application_new ("org.gtk", G_APPLICATION_FLAGS_NONE);
  	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  	status = g_application_run (G_APPLICATION (app), argc, argv);
  	g_object_unref (app);

  	return status;
}
