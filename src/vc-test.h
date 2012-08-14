#define CLEAR(x) memset (&(x), 0, sizeof (x))


static void open_device(void);
static void errno_exit (const char *s);
static void init_mmap (void);
static void init_device (void);
static void start_capturing (void);
static void mainloop (void);
static void stop_capturing (void);
static void uninit_device (void);
static void close_device (void);

