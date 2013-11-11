#include "window.h"
#include "common.h"

static GtkWidget *window;
static GtkWidget *grid;
static GtkImage *image_diff;
static rgb_ptr old_buf = NULL;
static rgb_ptr trash_buf = NULL;
static pthread_t thread;
static GtkWidget* start_btn;
static int need_to_save_image = FALSE;
static int updating_form = FALSE;
static int framenumber = 0;
extern char* dev_name;

int get_diff(rgb_ptr buf_new, rgb_ptr buf_old, int length) {
	if((!buf_new)||(!buf_old)) {
		return 0;
	}
	int i = 0, total = 0, changed = 1; // zero division
	while ((i + BPP_RGB24) < length) {
		int diff = abs(buf_new[i] - buf_old[i]);
		total++;
		if (diff > 15) {
//			buf_old[i + 2] = 255;
			changed++;
		}
		i+=BPP_RGB24;
	}
	return total / changed;
}

rgb_ptr get_red(rgb_ptr buf, int length) {
	if(!buf) {
		return NULL;
	}
	int bar_chart[255];
	int i = 0;
	while ((i + BPP_RGB24) < length) {
		char k = buf[i] - (buf[i + 1] + buf[i + 2]) / 2;
		unsigned char r = clip(255 - k);

		bar_chart[r] += 1;

		buf[i ] = r;
		buf[i + 1] = r;
		buf[i + 2] = r;
		i+=BPP_RGB24;
	}
	return NULL;
}

void refresh_image() {
	if (updating_form)
		return;
	if (trash_buf)
		free(trash_buf);
	rgb_ptr buf = get_image();
	if (old_buf) {
		int diff = get_diff(buf, old_buf, 640 * 480 * 3);
		if (diff < 100)
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
		char filename[10];
		itoa(framenumber++, filename);
		strcat(filename, ".jpeg");
//		write_JPEG_file(filename, 640, 480, old_buf, 50);
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
	capture c = {dev_name, 640, 480, &refresh_image };
	pthread_create(&thread, NULL, &startcapture, &c);
	gtk_widget_set_sensitive(start_btn, FALSE);
	g_timeout_add(100, (GSourceFunc) update_form, (gpointer) window);
	g_timeout_add(1000, (GSourceFunc) save_image, (gpointer) window);
	sleep(1);
}

void show_main_form(int argc, char *argv[]) {
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

void reverse(char s[]) {
	int i, j;
	char c;

	for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

void itoa(int n, char s[]) {
	int i, sign;

	if ((sign = n) < 0)
		n = -n;
	i = 0;
	do {
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}