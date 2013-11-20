#include "window.h"
#include <gtk/gtk.h>

static GtkWidget *window;
static GtkWidget *grid;
static GtkImage *image_diff;
static rgb_ptr old_buf = NULL;
static rgb_ptr trash_buf = NULL;
static pthread_t thread;
static GtkWidget* start_btn;
static int need_to_save_image = FALSE;
static int updating_form = FALSE;
static capture *cap = NULL;


void refresh_image_gtk() {
	if (updating_form)
		return;
	if (trash_buf)
		free(trash_buf);
	rgb_ptr buf = get_image();
	if (old_buf) {
		float diff = get_diff(buf, old_buf, 640 * 480 * 3);
		if (diff > 1)
			need_to_save_image = TRUE;
		trash_buf = old_buf;
	}
	old_buf = buf;
}

void quit(GtkWidget *widget, gpointer data) {
	cancel_capturing();
	gtk_widget_set_sensitive(start_btn, TRUE);
}

static gboolean save_image(GtkWidget *widget) {

	if (need_to_save_image) {
		char filename[20];
		get_file_name(filename);
		strcat(filename, ".jpeg");
		if (cap->do_save_image)
			write_JPEG_file(filename, 640, 480, old_buf, 50);
	}
	need_to_save_image = FALSE;
	return TRUE;
}

static gboolean update_form(GtkWidget *widget) {
	updating_form = TRUE;
	GdkPixbuf* p_old_buf = gdk_pixbuf_new_from_data(old_buf, GDK_COLORSPACE_RGB,
	FALSE, 8, 640, 480, 640 * 3, NULL, NULL);
	gtk_image_set_from_pixbuf(image_diff, p_old_buf);
	g_object_unref(p_old_buf);
	updating_form = FALSE;

	return TRUE;
}

void start(GtkWidget *widget, gpointer data) {
	pthread_create(&thread, NULL, &startcapture, cap);
	gtk_widget_set_sensitive(start_btn, FALSE);
	g_timeout_add(100, (GSourceFunc) update_form, (gpointer) window);
	g_timeout_add(1000, (GSourceFunc) save_image, (gpointer) window);
	sleep(1);
}

void show_main_form(int argc, char *argv[], capture c) {
	cap = &c;
	cap->refresh = refresh_image_gtk;
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW (window), "Grid");
	g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	gtk_container_set_border_width(GTK_CONTAINER (window), 10);

	grid = gtk_grid_new();

	gtk_container_add(GTK_CONTAINER (window), grid);

	char stub[480 * 640 * 3];
	GdkPixbuf* tmp = gdk_pixbuf_new_from_data((rgb_ptr) stub,
			GDK_COLORSPACE_RGB, FALSE, 8, 640, 480, 640 * 3, NULL, NULL);
	image_diff = (GtkImage*) gtk_image_new_from_pixbuf(tmp);
	gtk_grid_attach(GTK_GRID (grid), (GtkWidget*) image_diff, 0, 0, 1, 1);

	start_btn = gtk_button_new_with_label("Start");
	g_signal_connect(start_btn, "clicked", G_CALLBACK (start), NULL);
	gtk_grid_attach(GTK_GRID (grid), start_btn, 0, 1, 1, 1);

	GtkWidget * object = gtk_button_new_with_label("Exit");
	g_signal_connect(object, "clicked", G_CALLBACK (quit), NULL);
	gtk_grid_attach(GTK_GRID (grid), object, 0, 2, 1, 1);

	gtk_widget_show_all(window);

	gtk_main();
}


