#ifndef TYPES_H_
#define TYPES_H_

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string.h>

#define rgb_ptr unsigned char*
#define CLEAR(x) memset (&(x), 0, sizeof (x))
#define HISTORY_LENGHT	2
#define BPP_YUY2 2
#define BPP_YUY2_PIXEL 4
#define BPP_RGB24 3

typedef struct _buffer {
	void * start;
	size_t length;
} buffer;

typedef struct _capture {
	char* device;
	int weigth;
	int height;
	int do_save_image;
	void (*refresh)(void);
} capture;

void get_file_name(char*);

void* startcapture(void*);

char clip(int x);

float get_diff(rgb_ptr, rgb_ptr, int);

rgb_ptr get_red(rgb_ptr, int);

void reverse(char[]);

void itoa(int, char[]);

#endif
