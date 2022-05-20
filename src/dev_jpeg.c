#include "dev_jpeg.h"

/******************************************************************************
 * Function:    jpeg
 *
 * Description: 把rgb数据压缩成jpeg数据
 * Input:       dest  
 *              rgb 
 *              width  
 *              height
 *              quality
 * Output:      NULL
 * Returns:     0 success
 *              -1 fail
 ******************************************************************************/
void rgb_compress_jpeg(FILE* dest, uint8_t* rgb, uint32_t width, uint32_t height, int quality)
{
  JSAMPARRAY image;
  image = calloc(height, sizeof (JSAMPROW));
  for (size_t i = 0; i < height; i++) {
    image[i] = calloc(width * 3, sizeof (JSAMPLE));
    for (size_t j = 0; j < width; j++) {
      image[i][j * 3 + 0] = rgb[(i * width + j) * 3 + 0];
      image[i][j * 3 + 1] = rgb[(i * width + j) * 3 + 1];
      image[i][j * 3 + 2] = rgb[(i * width + j) * 3 + 2];
    }
  }

  struct jpeg_compress_struct compress;
  struct jpeg_error_mgr error;
  compress.err = jpeg_std_error(&error);
  jpeg_create_compress(&compress);
  jpeg_stdio_dest(&compress, dest);

  compress.image_width = width;
  compress.image_height = height;
  compress.input_components = 3;
  compress.in_color_space = JCS_RGB;
  jpeg_set_defaults(&compress);
  jpeg_set_quality(&compress, quality, TRUE);
  jpeg_start_compress(&compress, TRUE);
  jpeg_write_scanlines(&compress, image, height);
  jpeg_finish_compress(&compress);
  jpeg_destroy_compress(&compress);

  for (size_t i = 0; i < height; i++) {
    free(image[i]);
  }
  free(image);
}

int minmax(int min, int v, int max)
{
  return (v < min) ? min : (max < v) ? max : v;
}
#if 0
uint8_t* yuyv_to_rgb(uint8_t* yuyv, uint32_t width, uint32_t height)
{
  uint8_t* rgb = calloc(width * height * 3, sizeof (uint8_t));
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j += 2) {
      size_t index = i * width + j;
      int y0 = yuyv[index * 2 + 0] << 8;
      int u = yuyv[index * 2 + 1] - 128;
      int y1 = yuyv[index * 2 + 2] << 8;
      int v = yuyv[index * 2 + 3] - 128;
      rgb[index * 3 + 0] = minmax(0, (y0 + 359 * v) >> 8, 255);
      rgb[index * 3 + 1] = minmax(0, (y0 + 88 * v - 183 * u) >> 8, 255);
      rgb[index * 3 + 2] = minmax(0, (y0 + 454 * u) >> 8, 255);
      rgb[index * 3 + 3] = minmax(0, (y1 + 359 * v) >> 8, 255);
      rgb[index * 3 + 4] = minmax(0, (y1 + 88 * v - 183 * u) >> 8, 255);
      rgb[index * 3 + 5] = minmax(0, (y1 + 454 * u) >> 8, 255);
    }
  }
  return rgb;
}
#endif

/******************************************************************************
 * Function:    yuyv_to_rgb
 *
 * Description: 把rgb数据压缩成jpeg数据
 * Input:       rgb  
 *              yuyv 
 *              width  
 *              height
 *              
 * Output:      NULL
 * Returns:     0 success
 *              -1 fail
 ******************************************************************************/
void* yuyv_to_rgb(uint8_t *rgb,uint8_t* yuyv, uint32_t width, uint32_t height)
{
  //uint8_t* rgb = calloc(width * height * 3, sizeof (uint8_t));
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j += 2) {
      size_t index = i * width + j;
      int y0 = yuyv[index * 2 + 0] << 8;
      int u = yuyv[index * 2 + 1] - 128;
      int y1 = yuyv[index * 2 + 2] << 8;
      int v = yuyv[index * 2 + 3] - 128;
      rgb[index * 3 + 0] = minmax(0, (y0 + 359 * v) >> 8, 255);
      rgb[index * 3 + 1] = minmax(0, (y0 + 88 * v - 183 * u) >> 8, 255);
      rgb[index * 3 + 2] = minmax(0, (y0 + 454 * u) >> 8, 255);
      rgb[index * 3 + 3] = minmax(0, (y1 + 359 * v) >> 8, 255);
      rgb[index * 3 + 4] = minmax(0, (y1 + 88 * v - 183 * u) >> 8, 255);
      rgb[index * 3 + 5] = minmax(0, (y1 + 454 * u) >> 8, 255);
    }
  }
  //return rgb;
}