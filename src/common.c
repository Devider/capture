#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "common.h"

char clip(int x) {
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
		if (diff > 25) {
//			buf_old[i + 2] = 255;
			changed++;
		}
		i+=BPP_RGB24;
	}
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

static void get_tmp_file_name(char* buf, char* path) {
	buf = strcpy(buf, path);
	buf = strcat(buf, "tmp");
	buf = strcat(buf, ".jpg");
}

static void get_file_name(char* buf, char* path) {
	buf = strcpy(buf, path);
	time_t t;
	struct tm *timeptr;
	t = time(NULL);
	timeptr = localtime(&t);
	char fn[15];
	strftime(fn, sizeof(fn), "%Y%m%d%H%M%S", timeptr);
	buf = strcat(buf, fn);
	buf = strcat(buf, ".jpg");
}

void send_data(rgb_ptr buff, size_t buff_size) {
	char buf[] = "  ";
	int sock;
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(2351); // или любой другой порт...
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("connect");
		exit(2);
	}

	printf("Sending: %d bytes... ", IMG_SIZE);
	int i = send(sock, buff, buff_size, 0);
	printf("sent: %d bytes \n", i);
	recv(sock, buf, sizeof(buf), 0);

	close(sock);
}


void write_jpeg_file(rgb_ptr image_buffer, int quality) {

	struct jpeg_compress_struct cinfo;

	struct jpeg_error_mgr jerr;

	FILE * outfile;
	JSAMPROW row_pointer[1];
	int row_stride;

	char filename[256];
	get_file_name(filename, get_io_cfg()->path);

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_compress(&cinfo);

	if ((outfile = fopen(filename, "wb")) == NULL ) {
		fprintf(stderr, "can't open %s\n", filename);
		return;
	}
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = get_cap()-> weigth;
	cinfo.image_height = get_cap()-> height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, true);

	jpeg_start_compress(&cinfo, true);

	row_stride = get_cap()-> weigth * 3;

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);

	jpeg_destroy_compress(&cinfo);
}


void send_jpeg_data(rgb_ptr image_buffer, int quality) {

	struct jpeg_compress_struct cinfo;

	struct jpeg_error_mgr jerr;

	FILE * outfile;
	JSAMPROW row_pointer[1];
	int row_stride;

	char filename[256];
	get_tmp_file_name(filename, get_io_cfg()->tmp_path);


	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_compress(&cinfo);

	if ((outfile = fopen(filename, "wb")) == NULL ) {
		fprintf(stderr, "can't open %s\n", filename);
		return;
	}
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = get_cap()-> weigth;
	cinfo.image_height = get_cap()-> height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, true);

	jpeg_start_compress(&cinfo, true);

	row_stride = get_cap()-> weigth * 3;

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);

	jpeg_destroy_compress(&cinfo);

	FILE* f;
	f = fopen(filename, "r");
	int c;
	size_t s = 0;

	char* buff = malloc(1024 * 30);

	while((c = getc(f))!=EOF){
		buff[s++]=c;
	}

	fclose(f);
	send_data((rgb_ptr)buff, s);
	free(buff);
}


void process_data(rgb_ptr buf){
	if (get_io_cfg()->do_save_image)
		write_jpeg_file(buf, 50);
	if (get_io_cfg()->do_send)
		send_data(buf, IMG_WITGH*IMG_HEIGHT*BPP_RGB24);
}


