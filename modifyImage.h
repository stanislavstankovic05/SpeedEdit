#ifndef modifyImage_H
#define modifyImage_H


#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <string.h>


void convertBnW(int components, int width, int height, unsigned char **buffer){

  for(int row = 0; row < height; ++row){
    for(int column = 0; column < width; ++column){
      int index =  column * components;
      //modify each pixel using luminous weightning
      int red = (int)buffer[row][index] * 0.299;
      int blue = (int) buffer[row][index + 1] * 0.114;
      int green = (int) buffer[row][index + 2] * 0.587;
      //gray vallue
      int gray = red + blue + green;

      buffer[row][index] = gray;
      buffer[row][index + 1] = gray;
      buffer[row][index + 2] = gray;
    }
  }
  printf("modified\n");
}

void imgtoText(){

}
#endif
