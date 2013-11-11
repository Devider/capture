#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <assert.h>
#include "common.h"

#define _TRUE_ 1
#define _FALSE_ 0

int ioctl(int, int, void*);
int close(int);
static void close_device(void);
static void uninit_device(void);
static void init_device(void);
static void mainloop(void (*refresh)(void));
static rgb_ptr yuy2_to_rgb24();
static void stop_capturing(void);
static void start_capturing(void);
static void open_device(void);
