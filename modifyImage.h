#ifndef modifyImage_H
#define modifyImage_H


#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <string.h>

#define max(a,b) ((a) > (b) ? (a) : (b))
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
    //printf("oke\n");
    //row_pointer[0] = **buffer + cinfo.next_scanline * row_stride;
    row_pointer[0] = (*buffer)[cinfo.next_scanline];
    //printf("oke2\n");
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


      //printf("new r: %d  and c:%d -> %d %d %d\n", row, column, (*buffer)[row][index], (*buffer)[row][index+1], (*buffer)[row][index+2]);
    }
  }
    
  printf("modified\n");
}


void pixalateImage(int components, int width, int height, unsigned char **buffer, unsigned char ***newBuffer, int newWidth, int newHeight){

  /* --|--|--
   * --|--|--
   * ________
   * --|--|--
   * --|--|--
   *___________
   * --|--|--
   * --|--|--
   *
   *Above is how we convert a 6x6 matrix in a 3x3, we will see how many pixels we need to compress(width/newWidth x height/newWidth or 6/3 x 6/3 in this case)
    we either do the medium of all pixel values for the new value of the compressed pixel, or we choose the one with higher occurence
   * */
  int L = height/newHeight + 1;
  int l = width/newWidth + 1;
  
  //printf("l = %d, L = %d \n", l, L);
  *newBuffer = (unsigned char **)malloc(newHeight * sizeof(unsigned char *));
  for(int row = 0; row < newHeight; row++){
    (*newBuffer)[row] = (unsigned char *)malloc(newWidth * components);
  }

  int newRow = 0;
  int newColumn = 0;
  

  int colorFrecv[256];
  for(int row = 0; row < height; row += L){
    //printf("\n\n\n\n row = %d \n\n\n\n\n", newRow);
    newColumn = 0;
    for(int i = 0; i <= 256; ++i)
      colorFrecv[i] = 0;
    for(int column = 0; column < width; column += l){
        int curentValue_red = 0;
        int curentValue_green = 0;
        int curentValue_blue;
        for(int i = row; i < row + L; ++i){
          for(int j = column; j < column + l; ++j){
            curentValue_red += buffer[row][j * components];
            curentValue_blue += buffer[row][j * components + 1];
            curentValue_green += buffer[row][j * components + 2];
            //colorFrecv[buffer[row][j * components]]++;
          }
        }
        curentValue_red/=L*l;
        curentValue_green/=L*l;
        curentValue_blue/=L*l;
        /*int maxColor = -1;
        int maxFrecv = -1;
        for(int index = 0; index <= 256; ++index){
          if(colorFrecv[index] > maxFrecv){
            maxFrecv = colorFrecv[index];
            maxColor = index;
          }
        }*/
        (*newBuffer)[newRow][newColumn * components] = curentValue_red;
        (*newBuffer)[newRow][newColumn * components + 1] = curentValue_blue;
        (*newBuffer)[newRow][newColumn * components + 2] = curentValue_green;
        //printf("column %d = %d |", newColumn, curentValue);
        newColumn++;
    }
    newRow++;
  }
  //printf("height: %d \n", height);
}


//void imgtoText(int components, int width, int height, unsigned char **buffer, int newWidth, int newHeight){
void imgtoText(int components, int width, int height, unsigned char **buffer, int newWidth, int newHeight){
  /* observation, if red = blue = green, then the resulting color is a shade of gray
   the pixel values are in the interval [0,256] (black and white included), so for each shade of gray, we need
   256 symbols, but he have only 128 ascii code, and some are reserved
   0 and 256 will have their own symbols, the rest of the values will be divided in subsets, each subset, will have a ascii symbol 
  each subset will have 25 numbers => ~ 11 symbols
  */
  int colorMap[256];
  colorMap[0]=32; //ascii code for _ (blank space)
  colorMap[256]=35;
  for (int value = 1; value <= 40; ++value)
    colorMap[value] = 32;

  for (int value = 40; value <= 65; ++value)
    colorMap[value] = '_';//36
  
  for (int value = 66; value <= 90; ++value)
    colorMap[value] = '-';//64

  for (int value = 91; value <= 120; ++value)
    colorMap[value] = '+';//64

  for (int value = 121; value <= 200; ++value)
    colorMap[value] = '.';//43

  for (int value = 200; value <= 256; ++value)
    colorMap[value] = ',';//37


  /* --|--|--
   * --|--|--
   * ________
   * --|--|--
   * --|--|--
   *___________
   * --|--|--
   * --|--|--
   *
   *Above is how we convert a 6x6 matrix in a 3x3, we will see how many pixels we need to compress(width/newWidth x height/newWidth or 6/3 x 6/3 in this case)
    we either do the medium of all pixel values for the new value of the compressed pixel, or we choose the one with higher occurence
   * */

  int L = height/newHeight;
  int l = width/newWidth;
  
  for(int row = 0; row < height; row += L){
    for(int column = 0; column < width; column += l){
        int curentValue = 0;
        for(int i = row; i < row + L; ++i){
          for(int j = column; j < column + l; ++j){
            curentValue += buffer[row][j * components];
          }
        }
        curentValue/=L*l; 
        printf("%c", colorMap[curentValue]);
    }
    printf("\n");
  }

  printf("\n");
  for(int row = 0; row < height; row++){
    for(int column=0; column < height; column++){
      int index = column * components;
      int value = buffer[row][index];
      printf("%c", colorMap[value]);
    }
    printf("\n");
  }
} 

#endif
