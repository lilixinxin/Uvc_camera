#ifndef __DEV_JPEG_H__
#define __DEV_JPEG_H__

#include "jpeglib.h"
#include "jconfig.h"
#include "jerror.h"
#include "jmorecfg.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
/**/
void rgb_compress_jpeg(FILE* dest, uint8_t* rgb, uint32_t width, uint32_t height, int quality);
//uint8_t* yuyv_to_rgb(uint8_t* yuyv, uint32_t width, uint32_t height);
void* yuyv_to_rgb(uint8_t *rgb,uint8_t* yuyv, uint32_t width, uint32_t height);
int minmax(int min, int v, int max);

#endif