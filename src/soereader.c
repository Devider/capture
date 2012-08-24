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

int main(int argn, char* argv[]) {
	startcapture("/dev/video0", 640, 480);
	puts("\nDone!\n");
	return EXIT_SUCCESS;
}
