#ifndef _VIBE_H_
#define _VIBE_H_

#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <process.h>
#include <iostream>
#include<opencv2\opencv.hpp>

typedef struct
{
	unsigned char *ptr_samples;
	unsigned int number_of_samples;
	unsigned int size_of_sample;
}Pixel;

typedef struct
{
	Pixel *ptr_pixels;
	unsigned int width;
	unsigned int height;
	unsigned int stride;
	unsigned int number_of_samples;
	unsigned int matching_threshold;
	unsigned int matching_number;
	unsigned int update_factor;
}VibeModel;



static unsigned int *ptr_rnd;
static unsigned int rnd_size;
static unsigned int rnd_position;

unsigned char get_rand_pixel(const unsigned char *image_data, const unsigned int width, const unsigned int height, const unsigned int stride, const unsigned int x, const unsigned int y);
int32_t LibVibeModelInit(VibeModel *model, const unsigned char *image_data, const unsigned int width, const unsigned int height, const unsigned int stride);
int32_t LibVibeModelUpdate(VibeModel *model, const unsigned char *image_data, unsigned char *segmentation_map);


void InitRnd(unsigned int size);
unsigned int freeRnd();
#endif /*__CV_VIBE_H__*/
