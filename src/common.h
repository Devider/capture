#ifndef COMMON_H_
#define COMMON_H_

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <setjmp.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <jpeglib.h>



#define rgb_ptr unsigned char*
#define CLEAR(x) memset (&(x), 0, sizeof (x))
#define HISTORY_LENGHT	2
#define BPP_YUY2 2
#define BPP_YUY2_PIXEL 4
#define BPP_RGB24 3
#define IMG_WITGH 640
#define IMG_HEIGHT 480
#define IMG_SIZE IMG_HEIGHT*IMG_WITGH*BPP_RGB24

typedef struct _capture {
	char* device;
	int weigth;
	int height;
	int bpp;
	void (*refresh)(void);
} capture;

typedef struct _io_cfg {
	char* path;
	char* tmp_path;
	int do_save_image;
	int do_send;
} iocfg;


typedef struct _buffer {
	void * start;
	size_t length;
} buffer;

void write_jpeg_file(rgb_ptr, int);

void send_jpeg_data(rgb_ptr, int);

void process_data(rgb_ptr);

void* startcapture(void*);

char clip(int x);

float get_diff(rgb_ptr, rgb_ptr, int);

rgb_ptr get_red(rgb_ptr, int);

void reverse(char[]);

void itoa(int, char[]);

void send_data(rgb_ptr buff, size_t buff_size);

void save_image_to_file(rgb_ptr old_buf);

capture* get_cap();

iocfg* get_io_cfg();

#endif
