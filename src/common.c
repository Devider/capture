#include "common.h"
#include <time.h>


char clip(int x){
	return x > 255 ? 255 : x < 0 ? 0 : x;
}

float get_diff(rgb_ptr buf_new, rgb_ptr buf_old, int length) {
	if((!buf_new)||(!buf_old)) {
		return 0;
	}
	int i = 0, total = 0, changed = 1; // zero division
	while ((i + BPP_RGB24) < length) {
		int diff = abs((buf_new[i] - buf_old[i]));
		total++;
		if (diff > 17) {
//			buf_old[i + 2] = 255;
			changed++;
		}
		i+=BPP_RGB24;
	}
//	printf("total %d  changed %d  persents %f \n", total, changed, (float)changed / total * 100);
	return (float)changed / total * 100;
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

void get_file_name(char* buf, char* path){
	buf = strcpy(buf, path);
	time_t t;
	struct tm *timeptr;
	t = time(NULL);
	timeptr = localtime(&t);
	char fn[15];
	strftime(fn, sizeof(fn), "%Y%m%e%H%M%S", timeptr);
	buf = strcat(buf, fn);
	buf = strcat(buf, ".jpg");
}
