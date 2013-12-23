#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "window.h"
#include "console.h"

char *frontend = "gtk";
char *dev_name = "/dev/video0";
char *path = "/data/capture/";
char *tmp_path = "/tmp/";
static const char short_options[] = "f:d:po:ts";

static const struct option long_options[] =
{{ "frontend", required_argument, NULL, 'f' },
 { "device", required_argument, NULL, 'd' },
 { "out", required_argument, NULL, 'o' },
 { "help", no_argument, NULL, 'h' },
 { "pretend", no_argument, NULL, 'p' },
 { "tmp", no_argument, NULL, 't' },
 { "silent", no_argument, NULL, 's' },
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
			"-t | --tmp              Path to /tmp images [%s]\n"
			"-s | --silent           Do not send by network\n"
			"", argv[0], dev_name, frontend, path, tmp_path);
}

int main(int argn, char* argv[]) {
	int idx;
	int c;
	int do_save = true;
	int do_send = true;
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
		case 's':
			do_send = false;
			break;
		case 'p':
			do_save = false;
			break;
		case 't':
			tmp_path = optarg;
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
	capture *cap = init_cap(dev_name, IMG_WITGH, IMG_HEIGHT, BPP_RGB24);
	iocfg *cfg = init_iocfg(path, tmp_path, do_save, do_send);


	if (0 == strcmp(frontend, "gtk")) {
		show_main_form(argn, argv);
	} else if (0 == strcmp(frontend, "web")) {
		fprintf(stderr, "NOT IMPLEMENTED YET!");
	} else if (0 == strcmp(frontend, "cli")) {
		do_start_captirung_cli();
	} else {
		fprintf(stderr, "Only 'web', 'cli' or 'gtk' frontends are possible!");
	}
	return 0;
}
