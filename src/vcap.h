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
#include "types.h"

#define _TRUE_ 1
#define _FALSE_ 0

int ioctl(int, int, void*);
int close(int);
