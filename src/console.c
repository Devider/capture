#include "common.h"
#include <pthread.h>


static pthread_t thread;
unsigned int sleep (unsigned int);
extern char* dev_name;
static short frame_is_reafy = 0;

void refresh_image_cli();

void start_cli() {
	capture c = {dev_name, 640, 480, &refresh_image_cli };
	pthread_create(&thread, 0, &startcapture, &c);
	sleep(1);
	while(1){
		sleep(1);
		if(frame_is_reafy){

		}
	}
	sleep(1);
}

void refresh_image_cli() {
	frame_is_reafy = 1;
}
