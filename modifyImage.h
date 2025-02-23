#ifndef modifyImage_H
#define modifyImage_H


#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <string.h>


void exportImage(int components, int width, int height, unsigned char ***buffer, char *exportFile){

  /* start compressing*/
  
  //basic structures
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  //

  FILE *outputImage = fopen(exportFile, "wb");
  int row_stride;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  jpeg_stdio_dest(&cinfo, outputImage);

  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = components;
  //cinfo.in_color_space = JCS_GRAYSCALE;
  cinfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&cinfo);
  
  int quality = 90; //default 
  jpeg_set_quality(&cinfo, quality, TRUE);
  
  jpeg_start_compress(&cinfo, TRUE);
  row_stride = components * width;

  JSAMPROW row_pointer[1];
  int row = 0;
   while (cinfo.next_scanline < cinfo.image_height) { 
    printf("oke\n");
    row_pointer[0] = **buffer + cinfo.next_scanline * row_stride;
    printf("oke2\n");
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
   }
   jpeg_finish_compress(&cinfo);
   jpeg_destroy_compress(&cinfo);
  printf("export completed\n");
}
void convertBnW(int components, int width, int height, unsigned char ***buffer){

  for(int row = 0; row < height; ++row){
    for(int column = 0; column < width; ++column){
      int index =  column * components;
    
      //modify each pixel using luminous weightning
      int red = (*buffer)[row][index] * 0.299;
      int blue = (*buffer)[row][index + 1] * 0.114;
      int green = (*buffer)[row][index + 2] * 0.587;
      //gray vallue
      int gray = red + blue + green;

      (*buffer)[row][index] = gray;
      (*buffer)[row][index + 1] = gray;
      (*buffer)[row][index + 2] = gray;


      printf("new r: %d  and c:%d -> %d %d %d\n", row, column, (*buffer)[row][index], (*buffer)[row][index+1], (*buffer)[row][index+2]);
    }
  }
    
  printf("modified\n");
}

void imgtoText(){

}
#endif
