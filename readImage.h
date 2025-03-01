#ifndef readImage_H
#define readImage_H

#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <string.h>

void readImage(char *filename, unsigned char ***buffer, int * imageComponents, int *width, int *height)
{
	FILE *image = fopen(filename, "rb");
	if(!image){
		printf("error");
		return;
	}
	//standard structures to read JPEG file
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	//the actuall reading process
	
	jpeg_stdio_src(&cinfo,image);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	int row_stride = cinfo.output_width * cinfo.output_components;
	// JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
	// 		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	

	unsigned char *rowBuffer = (unsigned char *) malloc(row_stride);

	*buffer = (unsigned char **)malloc(cinfo.image_height * sizeof(unsigned char *));
    
	int row=0;
	
	*imageComponents=cinfo.output_components;
	*width=cinfo.image_width;
	*height=cinfo.output_height;

	while (cinfo.output_scanline < cinfo.output_height) {
    	jpeg_read_scanlines(&cinfo, &rowBuffer, 1);
		
		(*buffer)[row] = (unsigned char *)malloc(row_stride);
		memcpy((*buffer)[row], rowBuffer, row_stride);

		for(int i=0;i<cinfo.image_width;++i){
			int index = i * cinfo.output_components;
 			//printf(" row %d column %d Pixel %d, %d, %d, \n", row, i, rowBuffer[index], rowBuffer[index+1], rowBuffer[index+2]);
		}
		row++;
  	}	


	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(image);
}


#endif
