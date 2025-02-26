#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <jpeglib.h>
#include <setjmp.h>


#include "readImage.h"
#include "modifyImage.h"


// void readImage(char *filename)
// {
// 	FILE *image = fopen(filename, "rb");
// 	if(!image){
// 		printf("error");
// 		return;
// 	}
// 	//standard structures to read JPEG file
// 	struct jpeg_decompress_struct imageInfo;
// 	struct jpeg_error_mgr jerr;

// 	imageInfo.err = jpeg_std_error(&jerr);
// 	jpeg_create_decompress(&imageInfo);

// 	//the actuall reading process
	
// 	jpeg_stdio_src(&imageInfo,image);
// 	jpeg_read_header(&imageInfo, TRUE);
// 	jpeg_start_decompress(&imageInfo);

// 	int width = imageInfo.output_width;
// 	int numberComponents = imageInfo.output_components;
// 	printf("%dx \n", width);
	
// 	unsigned char *rowBuffer = (unsigned char *) malloc(width *numberComponents);
// 	jpeg_read_scanlines(&imageInfo, &rowBuffer,1);
// 	for (int i=0; i < width ; ++i){
// 		int index = i * numberComponents;
// 		printf("Pixel %d, %d, %d, \n", rowBuffer[index], rowBuffer[index+1],rowBuffer[index+2]);
// 	}

// }
int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		printf("error, not enough parameters\n");
		return 0;
	}
	
	unsigned char **buffer;
	int imageComponents=0;
	int imgWidth = 0;
	int imgHeight = 0;
	readImage(argv[1], &buffer, &imageComponents, &imgWidth, &imgHeight);
	printf("%d %d %d \n", imageComponents, imgWidth, imgHeight);

  for (int row = 0; row < imgHeight; ++row) {
        for (int column = 0; column < imgWidth; ++column) {
            int index = column * imageComponents;
            printf("Row %d Column %d Pixel R=%d G=%d B=%d\n", row, column, buffer[row][index], buffer[row][index + 1], buffer[row][index + 2]);
        }
  }

  convertBnW(imageComponents, imgWidth, imgHeight, &buffer);
  exportImage(imageComponents, imgWidth, imgHeight, &buffer, argv[2]);
  imgtoText(imageComponents, imgWidth, imgHeight, buffer);
	return 0;
}
