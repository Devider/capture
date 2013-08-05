#include <gtk/gtk.h>
#include <stddef.h>
#include "types.h"

static GtkWidget *window;
static GtkWidget *grid;
static GtkImage *image_stream;
static GtkImage *image_diff;
static rgb_ptr old_buf = NULL;
static rgb_ptr trash_buf = NULL;

int abs(int);
void free(void*);
rgb_ptr get_image();
char clip(int);

void get_diff(rgb_ptr buf_new, rgb_ptr buf_old, int length) {
	if((!buf_new)||(!buf_old)){
		return;
	}
	int i = 0;
	while ((i + BPP_RGB24) < length) {
		int diff = abs(buf_new[i] - buf_old[i]);
		if (diff > 15)
			buf_old[i + 2] = 255;
		i+=BPP_RGB24;
	}
}

rgb_ptr get_red(rgb_ptr buf, int length) {
	if(!buf){
		return NULL;
	}
	int bar_chart[255];
	int i = 0;
	while ((i + BPP_RGB24) < length) {
		char k = buf[i] - (buf[i + 1] + buf[i + 2]) / 2;
		unsigned char r = clip(255 - k);

		bar_chart[r] += 1;

		buf[i    ] = r;
		buf[i + 1] = r;
		buf[i + 2] = r;
		i+=BPP_RGB24;
	}
	return NULL;
}

void refresh_image() {
	if (trash_buf)
		free(trash_buf);
	rgb_ptr buf = get_image();
	if (old_buf) {
		//get_diff(buf, old_buf, 640 * 480 * 3);
		get_red(old_buf, 640 * 480 * 3);
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

static gboolean time_handler(GtkWidget *widget) {
//	refresh_image();
//	puts("\Timer!\n");
	return TRUE;
}

static void quit(GtkWidget *widget, gpointer data) {
	gtk_main_quit();
}

static void start(GtkWidget *widget, gpointer data) {
	pthread_t thread;
	capture c = {"/dev/video0", 640, 480, refresh_image};
	pthread_create(&thread, NULL, &startcapture, &c);
	sleep(1);
	g_timeout_add(1000 / 25 , (GSourceFunc) time_handler, NULL );

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

	const char* file_name="/home/kirill/workspace.c/soereader/Debug/first.png";
	image_stream = (GtkImage*) gtk_image_new_from_file(file_name);
	gtk_grid_attach(GTK_GRID (grid), (GtkWidget*)image_stream, 0, 0, 1, 1);

	file_name="/home/kirill/workspace.c/soereader/Debug/second.png";
	image_diff = (GtkImage*) gtk_image_new_from_file(file_name);
	gtk_grid_attach(GTK_GRID (grid), (GtkWidget*)image_diff, 1, 0, 1, 1);

	object = gtk_button_new_with_label("Start");
	g_signal_connect(object, "clicked", G_CALLBACK (start), NULL);
	gtk_grid_attach(GTK_GRID (grid), object, 0, 1, 1, 1);

	object = gtk_button_new_with_label("Exit");
	g_signal_connect(object, "clicked", G_CALLBACK (quit), NULL);
	gtk_grid_attach(GTK_GRID (grid), object, 1, 1, 1, 1);

	gtk_widget_show_all(window);

	gtk_main();
}
