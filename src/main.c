#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

char	*frontend = "gtk";
char	*dev_name = "/dev/video0";

static const char short_options[] = "f:d:";

static const struct option
long_options[] = {
        { "interface", required_argument, NULL, 'f' },
        { "device", required_argument, NULL, 'd' },
        { 0, 0, 0, 0 }
};

static void usage(FILE *fp, int argc, char **argv)
{
        fprintf(fp,
                 "Usage: %s [options]\n\n"
                 "Version 0.3\n"
                 "Options:\n"
                 "-d | --device name   Video device name [%s]\n"
                 "-f | --interface name   Frontend (\"gtk\"|\"web\") [%s]\n"
                 "-h | --help          Print this message\n"
                 "",
                 argv[0], dev_name, frontend);
}


int main(int argn, char* argv[]) {
	int idx;
	int c;

	c = getopt_long(argn, argv, short_options, long_options, &idx);

	switch (c) {
	case 'f':
		frontend = optarg;
		break;
	case 'd':
		dev_name = optarg;
		break;
	default:
		usage(stderr, argn, argv);
		exit(-1);
	}
	if (0 == strcmp(frontend, "gtk")){
		capture c = {dev_name, 640, 480, NULL };
		show_main_form(argn, argv, c);
	} else if (0 == strcmp(frontend, "web")) {
		fprintf(stderr,"NOT IMPLEMENTED YET!");
	} else {
		fprintf(stderr,"Only 'web' or 'gtk' frontends are possible!");
	}
	return 0;
}
