#include "vcap.h"
#include <sys/time.h>

static void init_mmap(void);
static void errno_exit(const char *s);
static void close_device(void);
static void uninit_device(void);
static void init_device(void);
static void mainloop(void (*refresh)(void));
static void stop_capturing(void);
static void start_capturing(void);
static void open_device(void);
int ioctl(int, int, void*);
int close(int);

static rgb_ptr yuy2_to_rgb24();
static char * dev_name = NULL;
static int fd = -1;
static buffer * buffers = NULL;
static unsigned int n_buffers = 0;
static int wigth = 0;
static int heigth = 0;
static int capturing = 0;
static void* buffer_copy = NULL;
static int buffer_copy_lenght = 0;
static int can_stop = 1;

void* startcapture(void* dev) {
	can_stop = 1;
	capture* d = get_cap();
	dev_name = d->device;
	wigth = d->weigth;
	heigth = d->height;
	open_device();
	init_device();
	start_capturing();
	mainloop(d->refresh);
	stop_capturing();
	uninit_device();
	close_device();
	return 0;
}

rgb_ptr get_image() {
	capturing = _TRUE_;
	rgb_ptr result = yuy2_to_rgb24();
	capturing = _FALSE_;
	return result;
}

void cancel_capturing(){
	can_stop = 0;
}

static void yuv2rgb(int y, int u, int v, char *r, char *g, char *b) {
	int c = y - 16, d = u - 128, e = v - 128;

	int C = 1.164383 * c;
	*r = clip(C + 1.596027 * e);
	*g = clip(C - (0.391762 * d) - (0.812968 * e));
	*b = clip(C + 2.017232 * d);

}

static void yuyv_to_rgb(rgb_ptr buffer_yuyv, rgb_ptr buffer_rgb, int width, int height)
{
	rgb_ptr pixel_16;   // for YUYV
	rgb_ptr pixel_24;	// for RGB
	int y0, u0, v0, y1;
	char r, g, b;
	if ( buffer_yuyv == NULL || buffer_rgb == NULL)
	return;

	pixel_16 = (rgb_ptr)buffer_yuyv;//width * height * 2
	pixel_24 = buffer_rgb;//width * height * 3

	int i = 0, j = 0;
	while ((i + BPP_YUY2) < height * width * BPP_YUY2)
	{
		y0 = pixel_16[i];
		u0 = pixel_16[i+1];
		y1 = pixel_16[i+2];
		v0 = pixel_16[i+3];

		yuv2rgb(y0, u0, v0, &r, &g, &b);	// 1st pixel

		pixel_24[j] = r;
		pixel_24[j + 1] = g;
		pixel_24[j + 2] = b;
		j+=BPP_RGB24;

		yuv2rgb(y1, u0, v0, &r, &g, &b);// 2nd pixel

		pixel_24[j] = r;
		pixel_24[j + 1] = g;
		pixel_24[j + 2] = b;
		j+=BPP_RGB24;
		i += BPP_YUY2_PIXEL;
	}
}

//static void yuy2_to_rgb24_grey(rgb_ptr buffer_copy, rgb_ptr result){
//	int i = 0, j = 0;
//	rgb_ptr b = buffer_copy;
//	while ((i + BPP_YUY2) < buffer_copy_lenght) {
//		result[j] = b[i];
//		result[j + 1] = b[i];
//		result[j + 2] = b[i];
//		j+=BPP_RGB24;
//		i+=BPP_YUY2;
//	}
//}

static rgb_ptr yuy2_to_rgb24() {
	int newsize = buffer_copy_lenght / BPP_YUY2 * BPP_RGB24;
	rgb_ptr result = malloc(newsize);
	if (result == NULL) {
		printf("FAILED!!");
		fflush(stdout);
	}

	yuyv_to_rgb(buffer_copy, result, wigth, heigth);
	return result;
}

static void process_image(const buffer * buf) {
	if (!capturing) {
		memcpy(buffer_copy, buf->start, buf->length);
	} else {
		printf("busy...\n");
	}
}

static int xioctl(int fd, int request, void * arg) {
	int r;

	do
		r = ioctl(fd, request, arg);
	while (-1 == r && EINTR == errno);

	return r;
}

static void close_device(void) {
	if (-1 == close(fd))
		errno_exit("close");

	fd = -1;
}

static void uninit_device(void) {
	unsigned int i;

	for (i = 0; i < n_buffers; ++i)
		if (-1 == munmap(buffers[i].start, buffers[i].length))
			errno_exit("munmap");

	free(buffers);
	if (buffer_copy)
		free(buffer_copy);
}

static void init_device(void) {
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;

	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s is no V4L2 device\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "%s is no video capture device\n", dev_name);
		exit(EXIT_FAILURE);
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		fprintf(stderr, "%s does not support streaming i/o\n", dev_name);
		exit(EXIT_FAILURE);
	}

	/* Select video input, video standard and tune here. */

	CLEAR(cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect; /* reset to default */

		if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
			switch (errno) {
			case EINVAL:
				/* Cropping not supported. */
				break;
			default:
				/* Errors ignored. */
				break;
			}
		}
	} else {
		/* Errors ignored. */
	}

	CLEAR(fmt);

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = wigth;
	fmt.fmt.pix.height = heigth;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
		errno_exit("VIDIOC_S_FMT");

	/* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	init_mmap();
}

static void errno_exit(const char *s) {
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));

	exit(EXIT_FAILURE);
}

static int read_frame() {
	struct v4l2_buffer buf;

	CLEAR(buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
		switch (errno) {
		case EAGAIN:
			return 0;

		case EIO:
			/* Could ignore EIO, see spec. */

			/* fall through */

		default:
			errno_exit("VIDIOC_DQBUF");
			break;
		}
	}

	assert(buf.index < n_buffers);

	process_image(&buffers[buf.index]);
	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
		errno_exit("VIDIOC_QBUF");

	return 1;
}

static void mainloop(void (*refresh)(void)) {

	struct timeval last, current;
	gettimeofday(&last, NULL);
	gettimeofday(&current, NULL);

	while (can_stop) {
		for (;;) {
			fd_set fds;
			struct timeval tv;
			int r;

			FD_ZERO(&fds);
			FD_SET(fd, &fds);

			/* Timeout. */
			tv.tv_sec = 2;
			tv.tv_usec = 0;

			r = select(fd + 1, &fds, NULL, NULL, &tv);

			if (-1 == r) {
				if (EINTR == errno)
					continue;

				errno_exit("select");
			}

			if (0 == r) {
				fprintf(stderr, "select timeout\n");
				exit(EXIT_FAILURE);
			}
			if (read_frame()){
				break;
			}
		}

		gettimeofday(&current, NULL);

		long seconds  = current.tv_sec  - last.tv_sec;
		long useconds = current.tv_usec - last.tv_usec;

		long mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

		if (mtime > 200){
			refresh();
			gettimeofday(&last, NULL);
		}

	}
}

static void stop_capturing(void) {
	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
		errno_exit("VIDIOC_STREAMOFF");
}

static void start_capturing(void) {
	unsigned int i;
	enum v4l2_buf_type type;

	for (i = 0; i < n_buffers; ++i) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
			errno_exit("VIDIOC_QBUF");
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
		errno_exit("VIDIOC_STREAMON");
}

static void init_mmap(void) {
	struct v4l2_requestbuffers req;

	buffer_copy_lenght = wigth * heigth * BPP_YUY2;
	buffer_copy = malloc(buffer_copy_lenght);
	CLEAR(req);

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
					"memory mapping\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
		exit(EXIT_FAILURE);
	}

	buffers = calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;

		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;

		buffers[n_buffers].start = mmap(NULL /* start anywhere */, buf.length,
				PROT_READ | PROT_WRITE /* required */,
				MAP_SHARED /* recommended */, fd, buf.m.offset);

		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit("mmap");
	}
}

static void open_device(void) {
	struct stat st;

	if (-1 == stat(dev_name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno,
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		exit(EXIT_FAILURE);
	}

	fd = open(dev_name, O_RDWR /* required */| O_NONBLOCK, 0);

	if (-1 == fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno,
				strerror(errno));
		exit(EXIT_FAILURE);
	}
}

