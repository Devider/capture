#include <stdio.h>
#include "jpeglib.h"
#include <setjmp.h>
#include "types.h"

int exit(int);

void write_JPEG_file(char * filename, int image_width, int image_height,
		const rgb_ptr image_buffer, int quality) {

			struct jpeg_compress_struct cinfo;

			struct jpeg_error_mgr jerr;

			FILE * outfile;
			JSAMPROW row_pointer[1];
			int row_stride;

			cinfo.err = jpeg_std_error(&jerr);

			jpeg_create_compress(&cinfo);

			if ((outfile = fopen(filename, "wb")) == NULL ) {
				fprintf(stderr, "can't open %s\n", filename);
				exit(1);
			}
			jpeg_stdio_dest(&cinfo, outfile);

			cinfo.image_width = image_width;
			cinfo.image_height = image_height;
			cinfo.input_components = 3;
			cinfo.in_color_space = JCS_RGB;
			jpeg_set_defaults(&cinfo);
			jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

			jpeg_start_compress(&cinfo, TRUE);

			row_stride = image_width * 3; /* JSAMPLEs per row in image_buffer */

			while (cinfo.next_scanline < cinfo.image_height) {
				row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
				(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
			}

			jpeg_finish_compress(&cinfo);
			fclose(outfile);

			jpeg_destroy_compress(&cinfo);
		}

typedef struct error_mgr {
	struct jpeg_error_mgr pub; /* "public" fields */

	jmp_buf setjmp_buffer; /* for return to caller */
}* error_ptr;

void error_exit(j_common_ptr cinfo) {
	error_ptr myerr = (error_ptr) cinfo->err;

	(*cinfo->err->output_message)(cinfo);

	longjmp(myerr->setjmp_buffer, 1);
}



/*rgb_ptr get_martix(rgb_ptr ptr, int length) {
	if (!(length % 3)) {
		int i = 0, j= 0;
		rgb_ptr result = malloc(length % 3);
		while ((i+=3) < length) {
			result[j++] = ptr[i];
		}
		return result;
	} else {
		return NULL;
	}
}*/
