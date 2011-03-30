/*
various utilities for extracting part of texture

Copyright (C) 2002 by AIR-IX SUUNNITTELU/Ahiplan Oy

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include "Python.h"
#include <gif_lib.h>

static PyObject *texture_sample(PyObject *self, PyObject *args)
{
/*
  sample part of texture, args:
  whole texture as string, width and height
  output texture width and height
  upper lext and lower right corbers of rectangle from whole texture to be sampled into output texture
  
*/
  long int textureWidth, textureHeight;
  long int screenWidth, screenHeight;
  int i;
  int textureLen;
  char *texture;
  char *result;
  int resultSize;
  char *resPtr;
  double x1,y1,x2,y2;
  double dx,dy;
  double texturey,texturex;
  int y,x,posy,posx;
  PyObject *pyRes;
  if (!PyArg_ParseTuple(args, "s#lllldddd",
                        &texture, &textureLen,
                        &textureWidth, &textureHeight,
                        &screenWidth, &screenHeight,
                        &x1, &y1, &x2, &y2))
    return NULL;
  resultSize = screenWidth*screenHeight*3;
/*
  printf("%li, %li, %li, %li, %g, %g, %g, %g (%i)\n",
         textureWidth, textureHeight,
         screenWidth, screenHeight,
         x1,y1,x2,y2,
         resultSize);
*/
  resPtr = result = malloc(resultSize);
  if(!resPtr) return NULL;
  dx = (x2-x1)/screenWidth;
  dy = (y2-y1)/screenHeight;
  //printf("d: %g,%g\n", dx,dy);
  texturey = y1;
  for(y=0; y<screenHeight; y++, texturey+=dy) {
    if(texturey<0.0 || texturey>=textureHeight) {
      for(x=0; x<screenWidth*3; x++, resPtr++) {
        *resPtr = -1;
      }
    } else {
      posy = (int)texturey * textureWidth*3;
      texturex = x1;
      for(x=0; x<screenWidth; x++, texturex+=dx) {
        if(texturex<0.0 || texturex>=textureWidth) {
          for(i=0; i<3; i++, resPtr++) *resPtr = -1;
        } else {
          posx = posy + (int)texturex*3;
          for(i=0; i<3; i++, resPtr++) {
            *resPtr = texture[posx+i];
            //printf("%i,%i %g,%g: %i\n", y,x, texturey, texturex, (int)texture[posx+i]);
          }
        }
      }
    }
  }
  //printf("...done\n");
  
  pyRes = PyString_FromStringAndSize(result, resultSize);
  free(result);
  return pyRes;
}

static PyObject *texture_combine(PyObject *self, PyObject *args)
{
  int textureLen;
  char *texture1, *texture2;
  char *result;
  PyObject *pyRes;
  int i;
  if(!PyArg_ParseTuple(args, "s#s#",
                       &texture1, &textureLen, &texture2, &textureLen))
    return NULL;
  result = malloc(textureLen);
  for(i=0; i<textureLen; i+=3) {
    if(texture2[i]==-1 && texture2[i+1]==-1 && texture2[i+2]==-1) {
      result[i] = texture1[i];
      result[i+1] = texture1[i+1];
      result[i+2] = texture1[i+2];
    } else {
      result[i] = texture2[i];
      result[i+1] = texture2[i+1];
      result[i+2] = texture2[i+2];
    }
  }
  pyRes = PyString_FromStringAndSize(result, textureLen);
  free(result);
  return pyRes;
}




#define failExit {PrintGifError();exit(EXIT_FAILURE);}
GifFileType *gifFileIn=NULL, *gifFileOut;

void init_gifs(char *fileName, int sizex, int sizey)
{
  if(gifFileIn==NULL) {
    gifFileIn = DGifOpenFileName("kaavyhdi.gif");
    if(gifFileIn==NULL) {failExit;}
    printf("%i,%i: %i\n", gifFileIn->SWidth, gifFileIn->SHeight, gifFileIn->SColorResolution);
    if(DGifSlurp(gifFileIn) == GIF_ERROR) failExit;
    printf("slurp done\n");
  }
  gifFileOut = EGifOpenFileName(fileName, FALSE);
  if(gifFileOut==NULL) failExit;

  printf("making output...\n");
  if(EGifPutScreenDesc(gifFileOut, sizex, sizey, gifFileIn->SColorResolution,
                       gifFileIn->SBackGroundColor, 
                       MakeMapObject(gifFileIn->SColorMap->ColorCount,
                                     gifFileIn->SColorMap->Colors)) == GIF_ERROR) failExit;
  if(EGifPutImageDesc(gifFileOut, 0, 0, sizex, sizey, FALSE, NULL) == GIF_ERROR) failExit;
}

#define WHITE_PIXEL 1
static PyObject *texture_sample_gif(PyObject *self, PyObject *args)
{
  long int textureWidth, textureHeight;
  long int screenWidth, screenHeight;
  int i;
  char *fileName;
  char *texture;
  char *result;
  int resultSize;
  char *resPtr;
  double x1,y1,x2,y2;
  double dx,dy;
  double texturey,texturex;
  int y,x,posy,posx;
  PyObject *pyRes;
  if (!PyArg_ParseTuple(args, "slllldddd",
                        &fileName,
                        &textureWidth, &textureHeight,
                        &screenWidth, &screenHeight,
                        &x1, &y1, &x2, &y2))
    return NULL;
  init_gifs(fileName, screenWidth, screenHeight);
  texture = gifFileIn->SavedImages[0].RasterBits;
  resultSize = screenWidth*screenHeight;
  resPtr = result = malloc(resultSize);
/*
  printf("%li, %li, %li, %li, %g, %g, %g, %g (%i)\n",
         textureWidth, textureHeight,
         screenWidth, screenHeight,
         x1,y1,x2,y2,
         resultSize);
*/
  if(!resPtr) return NULL;
  dx = (x2-x1)/screenWidth;
  dy = (y2-y1)/screenHeight;
  //printf("d: %g,%g\n", dx,dy);
  texturey = y1;
  for(y=0; y<screenHeight; y++, texturey+=dy) {
    if(texturey<0.0 || texturey>=textureHeight) {
      for(x=0; x<screenWidth; x++, resPtr++) *resPtr = WHITE_PIXEL;
    } else {
      posy = (int)texturey * textureWidth;
      texturex = x1;
      for(x=0; x<screenWidth; x++, texturex+=dx) {
        if(texturex<0.0 || texturex>=textureWidth) {
          *resPtr++ = WHITE_PIXEL;
        } else {
          posx = posy + (int)texturex;
          *resPtr++ = texture[posx];
          //printf("%i,%i %g,%g: %i\n", y,x, texturey, texturex, (int)texture[posx+i]);
        }
      }
    }
  }
  //printf("...done\n");
  
  if(EGifPutLine(gifFileOut, result, resultSize) == GIF_ERROR) failExit;
  free(result);
  if(EGifCloseFile(gifFileOut) == GIF_ERROR) failExit;
  return Py_None;
}


static PyMethodDef TextureMethods[] = {
  {"sample",  texture_sample, METH_VARARGS},
  {"combine",  texture_combine, METH_VARARGS},
  {"sample_gif",  texture_sample_gif, METH_VARARGS},
  {NULL,      NULL}        /* Sentinel */
};

void inittexture()
{
  (void) Py_InitModule("texture", TextureMethods);
}
