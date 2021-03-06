#include "window.h"
#include "common.h"
#include <gtk/gtk.h>

static GtkWidget *window;
static GtkWidget *grid;
static GtkImage *image_diff;
static rgb_ptr old_buf = NULL;
static pthread_t thread;
static GtkWidget* start_btn;
static int need_to_save_image = FALSE;
static int blocked = false;

void refresh_image_gtk() {
	if (!need_to_save_image) {
		rgb_ptr buf = get_image();
		if (old_buf) {
			float diff = get_diff(buf, old_buf, IMG_WITGH * IMG_HEIGHT * 3);
			if (diff > 1)
			need_to_save_image = TRUE;
			free(old_buf);
		}
		old_buf = buf;
	}
}

void quit(GtkWidget *widget, gpointer data) {
	cancel_capturing();
	gtk_widget_set_sensitive(start_btn, TRUE);
}

static void check_image(GtkWidget *widget) {
	SYNCHRONIZED_BEGIN(blocked)
	if (need_to_save_image) {
		if (get_io_cfg()->do_save_image){
			process_data(old_buf);
		}
	}
	need_to_save_image = FALSE;
	SYNCHRONIZED_END(blocked)
}

static void update_form(GtkWidget *widget) {
	SYNCHRONIZED_BEGIN(blocked)
	GdkPixbuf* p_old_buf = gdk_pixbuf_new_from_data(old_buf, GDK_COLORSPACE_RGB,
	FALSE, 8, IMG_WITGH, IMG_HEIGHT, IMG_WITGH * 3, NULL, NULL);
	gtk_image_set_from_pixbuf(image_diff, p_old_buf);
	g_object_unref(p_old_buf);
	SYNCHRONIZED_END(blocked)
}

void start(GtkWidget *widget, gpointer data) {
	pthread_create(&thread, NULL, &startcapture, NULL);
	gtk_widget_set_sensitive(start_btn, FALSE);
	g_timeout_add(100, (GSourceFunc) update_form, (gpointer) window);
	g_timeout_add(1000, (GSourceFunc) check_image, (gpointer) window);
	sleep(1);
}

void show_main_form(int argc, char *argv[]) {
	get_cap()->refresh = refresh_image_gtk;
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Grid");
	g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	grid = gtk_grid_new();

	gtk_container_add(GTK_CONTAINER(window), grid);

	char stub[IMG_HEIGHT * IMG_WITGH * 3];
	GdkPixbuf* tmp = gdk_pixbuf_new_from_data((rgb_ptr) stub,
			GDK_COLORSPACE_RGB, FALSE, 8, IMG_WITGH, IMG_HEIGHT, IMG_WITGH * 3, NULL, NULL);
	image_diff = (GtkImage*) gtk_image_new_from_pixbuf(tmp);
	gtk_grid_attach(GTK_GRID(grid), (GtkWidget*) image_diff, 0, 0, 1, 1);

	start_btn = gtk_button_new_with_label("Start");
	g_signal_connect(start_btn, "clicked", G_CALLBACK (start), NULL);
	gtk_grid_attach(GTK_GRID(grid), start_btn, 0, 1, 1, 1);

	GtkWidget * object = gtk_button_new_with_label("Exit");
	g_signal_connect(object, "clicked", G_CALLBACK (quit), NULL);
	gtk_grid_attach(GTK_GRID(grid), object, 0, 2, 1, 1);

	gtk_widget_show_all(window);

	gtk_main();
}

