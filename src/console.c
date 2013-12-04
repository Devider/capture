#include <stdbool.h>
#include "common.h"
#include "vcap.h"

static rgb_ptr old_buf = NULL;
static pthread_t thread;
static int need_to_save_image = 0;
static capture *cap = NULL;

void refresh_image_cli() {
	if (!need_to_save_image) {
		rgb_ptr buf = get_image();
		int size = IMG_WITGH * IMG_HEIGHT * BPP_RGB24;
		if (old_buf) {
			float diff = get_diff(buf, old_buf, size);
			if (diff > 1) {
				need_to_save_image = true;
			}
			free(old_buf);
		}
		old_buf = buf;
	}
}

static void save_image() {

	if (need_to_save_image) {
		char filename[256];
		get_file_name(filename, cap->path);
		if (cap->do_save_image){
			write_JPEG_file(filename, IMG_WITGH, IMG_HEIGHT, old_buf, 50);
//			send_data(old_buf, IMG_WITGH*IMG_HEIGHT*BPP_RGB24);
		}
	}
	need_to_save_image = false;
}

void do_start_captirung_cli(capture c) {
	cap = &c;
	cap->refresh = refresh_image_cli;

	pthread_create(&thread, NULL, &startcapture, cap);
	while (1) {
		sleep(1);
		save_image();
	}
}
