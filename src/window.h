#include <gtk/gtk.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

void reverse(char[]);
void itoa(int, char[]);
int abs(int);
void free(void*);
rgb_ptr get_image();
char clip(int);
void cancel_capturing();
void write_JPEG_file(char*, int, int, rgb_ptr, int);

