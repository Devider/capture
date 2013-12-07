#include "common.h"

static capture* cap;
static iocfg* io_cfg;

capture* init_cap(char* dev_name, int witgh, int height, int bpp){
	capture* result = malloc((sizeof(struct _capture)));
	result-> device = dev_name;
	result-> weigth = witgh;
	result-> height = height;
	result-> bpp = bpp;
	cap = result;
    return result;
}

iocfg* init_iocfg(char* path, char* tmp_path, int do_save_image, int do_send){
	iocfg* result = malloc((sizeof(struct _io_cfg)));
	result-> path = path;
	result-> tmp_path = tmp_path;
	result-> do_save_image = do_save_image;
	result-> do_send = do_send;
	io_cfg = result;
    return result;
}


capture* get_cap(){return cap;}

iocfg* get_io_cfg(){return io_cfg;}
