#include <gtk/gtk.h>
#include "types.h"

static GtkWidget *window;
static GtkWidget *grid;
static GtkImage *image_stream;
static GtkImage *image_diff;
static rgb_ptr old_buf = NULL;
static rgb_ptr trash_buf = NULL;

static void get_diff(rgb_ptr buf_new, rgb_ptr buf_old, int length) {
	if((!buf_new)||(!buf_old)){
		return;
	}
	int i = 0;
	while ((i + 3) < length) {
		int diff = abs(*(buf_new + i) - *(buf_old + i));
		if (diff > 15)
			buf_old[i + 2] = 255;
		i+=3;
	}
}

static int refresh_image() {
	if (trash_buf)
		free(trash_buf);
	rgb_ptr buf = get_image();
	if (old_buf) {
		get_diff(buf, old_buf, 640 * 480 * 3);
		GdkPixbuf* p_old_buf = gdk_pixbuf_new_from_data(old_buf, GDK_COLORSPACE_RGB, FALSE, 8, 640, 480, 640 * 3, NULL, NULL);
		gtk_image_set_from_pixbuf(image_diff, p_old_buf);
		g_object_unref(p_old_buf);
		trash_buf = old_buf;
	}
	if (buf) {
		GdkPixbuf* p_buf = gdk_pixbuf_new_from_data(buf, GDK_COLORSPACE_RGB, FALSE, 8, 640, 480, 640 * 3, NULL, NULL);
		gtk_image_set_from_pixbuf(image_stream, p_buf);
		g_object_unref(p_buf);
	}
	old_buf = buf;
}

static void on_refresh_image(GtkWidget *widget, gpointer data) {

}

static gboolean time_handler(GtkWidget *widget) {
	refresh_image();
	return TRUE;
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
	g_signal_connect(object, "clicked", G_CALLBACK (on_refresh_image), NULL);

	gtk_grid_attach(GTK_GRID (grid), object, 0, 0, 1, 1);

	object = gtk_button_new_with_label("Button 2");
	g_signal_connect(object, "clicked", G_CALLBACK (quit), NULL);

	gtk_grid_attach(GTK_GRID (grid), object, 1, 0, 1, 1);

	image_stream = gtk_image_new_from_file(
			"/home/kirill/workspace/jpegtest/file0.jpg");

	gtk_grid_attach(GTK_GRID (grid), image_stream, 0, 1, 1, 1);

	image_diff = gtk_image_new_from_file(
			"/home/kirill/workspace/jpegtest/file1.jpg");

	gtk_grid_attach(GTK_GRID (grid), image_diff, 1, 1, 1, 1);

	g_timeout_add(1000 / 20 , (GSourceFunc) time_handler, NULL );

	gtk_widget_show_all(window);

	gtk_main();
}
