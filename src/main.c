#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "window.h"
#include "console.h"


char *frontend = "gtk";
char *dev_name = "/dev/video0";
char *path = "/data/capture";
static const char short_options[] = "f:d:po:";
capture cap;

static const struct option long_options[] =
{{ "frontend", required_argument, NULL, 'f' },
 { "device", required_argument, NULL, 'd' },
 { "out", required_argument, NULL, 'o' },
 { "help", no_argument, NULL, 'h' },
 { "pretend", no_argument, NULL, 'p' },
 { 0, 0, 0, 0 } };

static void usage(FILE *fp, int argc, char **argv) {
	fprintf(fp, "Usage: %s [options]\n\n"
			"Version 0.3\n"
			"Options:\n"
			"-d | --device name      Video device name [%s]\n"
			"-f | --frontend name    Frontend (\"gtk\"|\"web\"|\"cli\") [%s]\n"
			"-h | --help             Print this message\n"
			"-p | --pretend          Do not save nothing\n"
			"-o | --out              Path to save images [%s]\n"
			"", argv[0], dev_name, frontend, path);
}

int main(int argn, char* argv[]) {
	int idx;
	int c;
	int do_save = 1;
	int done_flag = 1;



	while (done_flag) {
		c = getopt_long(argn, argv, short_options, long_options, &idx);
		switch (c) {
		case 'f':
			frontend = optarg;
			break;
		case 'd':
			dev_name = optarg;
			break;
		case 'o':
			path = optarg;
			break;
		case 'p':
			do_save = 0;
			break;
		case 'h':
			usage(stderr, argn, argv);
			exit(0);
		case -1:
			done_flag = 0;
			break;
		default:
			usage(stderr, argn, argv);
			exit(-1);
		}
	}
	capture cap = { dev_name, path, IMG_WITGH, IMG_HEIGHT, do_save, NULL };
	if (0 == strcmp(frontend, "gtk")) {
		show_main_form(argn, argv, cap);
	} else if (0 == strcmp(frontend, "web")) {
		fprintf(stderr, "NOT IMPLEMENTED YET!");
	} else if (0 == strcmp(frontend, "cli")) {
		do_start_captirung_cli(cap);
	} else {
		fprintf(stderr, "Only 'web', 'cli' or 'gtk' frontends are possible!");
	}
	return 0;
}
