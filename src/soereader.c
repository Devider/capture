/*
 ============================================================================
 Name        : soereader.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "types.h"
int main(int argn, char* argv[]) {
	pthread_t thread;
	capture c = {"/dev/video0", 640, 480};
	pthread_create(&thread, NULL, &startcapture, &c);
	show_main_form(argn, argv);
	puts("\nDone!\n");
	return EXIT_SUCCESS;
}
