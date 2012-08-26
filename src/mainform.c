#include <gtk/gtk.h>
#include "types.h"

static GtkWidget *window;
static GtkWidget *grid;
static GtkImage *image;
static void* old_buf = NULL;

static void refresh_image(GtkWidget *widget, gpointer data) {
	rgb_ptr buf = get_image();
	if (old_buf)
		free(old_buf);
	GdkPixbuf* pbuf = gdk_pixbuf_new_from_data(buf, GDK_COLORSPACE_RGB, FALSE, 8, 640, 480, 640 * 3, NULL, NULL );
	gtk_image_set_from_pixbuf(image, pbuf);
	g_object_unref(pbuf);
	old_buf = buf;
	printf("attached!");
}
static void quit(GtkWidget *widget, gpointer data) {
	gtk_main_quit();
}
void show_main_form(int argc, char *argv[]) {
	GtkWidget *object;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW (window), "Grid");
	g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	gtk_container_set_border_width(GTK_CONTAINER (window), 10);

	grid = gtk_grid_new();

	gtk_container_add(GTK_CONTAINER (window), grid);

	object = gtk_button_new_with_label("Button 1");
	g_signal_connect(object, "clicked", G_CALLBACK (refresh_image), NULL);

	gtk_grid_attach(GTK_GRID (grid), object, 0, 0, 1, 1);

	object = gtk_button_new_with_label("Button 2");
	g_signal_connect(object, "clicked", G_CALLBACK (quit), NULL);

	gtk_grid_attach(GTK_GRID (grid), object, 1, 0, 1, 1);

	image = gtk_image_new_from_file("/home/kirill/workspace/jpegtest/file0.jpg");

	gtk_grid_attach(GTK_GRID (grid), image, 0, 1, 2, 1);

	gtk_widget_show_all(window);

	gtk_main();
}
