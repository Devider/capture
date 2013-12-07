
typedef struct _capture {
	char* device;
	int weigth;
	int height;
	int bpp;
	void (*refresh)(void);
} capture;

typedef struct _io_cfg {
	char* path;
	char* tmp_path;
	int do_save_image;
	int do_send;
} iocfg;

capture* init_cap(char*, int, int, int);

iocfg* init_iocfg(char*, char*, int, int);

capture* get_cap();

iocfg* get_io_cfg();
