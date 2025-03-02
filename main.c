#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <jpeglib.h>
#include <setjmp.h>


#include "readImage.h"
#include "modifyImage.h"



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
	//printf("%d %d %d \n", imageComponents, imgWidth, imgHeight);

  for (int row = 0; row < imgHeight; ++row) {
        for (int column = 0; column < imgWidth; ++column) {
            int index = column * imageComponents;
            //printf("Row %d Column %d Pixel R=%d G=%d B=%d\n", row, column, buffer[row][index], buffer[row][index + 1], buffer[row][index + 2]);
        }
  }

  convertBnW(imageComponents, imgWidth, imgHeight, &buffer);
  exportImage(imageComponents, imgWidth, imgHeight, &buffer, argv[2]);
  unsigned char **newBuffer;
  
  imgtoText(imageComponents, imgWidth, imgHeight, buffer, 50, 50);

  printf(" test pixalate\n");
  pixalateImage(imageComponents, imgWidth, imgHeight, buffer, &newBuffer, 50, 50);
  

  printf("pixalate succesfull\n");  
  exportImage(imageComponents, 50, 50, &newBuffer, argv[2]);
  printf("export succesfull");
  return 0;
}
