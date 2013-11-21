#include "common.h"
#include "vcap.h"


static rgb_ptr old_buf = NULL;
static rgb_ptr trash_buf = NULL;
static rgb_ptr buf_2_save = NULL;
static pthread_t thread;
static int need_to_save_image = 0;
static capture *cap = NULL;


void refresh_image_cli() {
	if (trash_buf)
		free(trash_buf);
	rgb_ptr buf = get_image();
	int size = 640 * 480 * 3;
	if (old_buf) {
		if(!need_to_save_image){
			float diff = get_diff(buf, old_buf, size);
			if (diff > 1){
				if (buf_2_save)
					free(buf_2_save);
				buf_2_save = malloc(size);
				memcpy(buf_2_save, buf, size);
				need_to_save_image = TRUE;
			}
		}
		trash_buf = old_buf;
	}
	old_buf = buf;
}


static void save_image() {

	if (need_to_save_image) {
		char filename[256];
		get_file_name(filename, cap->path);
		if (cap->do_save_image)
			write_JPEG_file(filename, 640, 480, buf_2_save, 50);
	}
	need_to_save_image = FALSE;
}

void do_start_captirung_cli(capture c) {
	cap = &c;
	cap->refresh = refresh_image_cli;

	pthread_create(&thread, NULL, &startcapture, cap);
	while(1){
		sleep(1);
		save_image();
	}
}
