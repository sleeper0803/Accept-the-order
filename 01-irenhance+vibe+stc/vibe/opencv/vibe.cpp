#include"vibe.h"

void InitRnd(unsigned int size)
{
	unsigned int seed = time(0);
	srand(seed);
	rnd_size = size;
	ptr_rnd = (unsigned int*)calloc(rnd_size, sizeof(unsigned int));
	for (unsigned int i = 0; i < rnd_size; i++)
	{
		ptr_rnd[i] = rand();
	}
	rnd_position = 0;
}

unsigned int freeRnd()
{
	free(ptr_rnd);
	rnd_size = 0;
	return 0;
}


unsigned char get_rand_pixel(const unsigned char *image_data, const unsigned int width, const unsigned int height, const unsigned int stride, const unsigned int x, const unsigned int y)
{
	unsigned int neighborRange = 1;
	int dx;
	int dy;
	dx = (x - neighborRange) + ptr_rnd[rnd_position = (rnd_position + 1) % rnd_size] % (2 * neighborRange);
	dy = (y - neighborRange) + ptr_rnd[rnd_position = (rnd_position + 1) % rnd_size] % (2 * neighborRange);
	if ((dx < 0) || (dx >= width))
	{
		if (dx < 0)
		{
			dx = ptr_rnd[rnd_position = (rnd_position + 1) % rnd_size] % (x + neighborRange);
		}
		else
		{
			dx = (x - neighborRange) + ptr_rnd[rnd_position = (rnd_position + 1) % rnd_size] % (width - x + neighborRange - 1);
		}
	}
	if ((dy < 0) || (dy >= height))
	{
		if (dy < 0)
		{
			dy = ptr_rnd[rnd_position = (rnd_position + 1) % rnd_size] % (y + neighborRange);
		}
		else
		{
			dy = (y - neighborRange) + ptr_rnd[rnd_position = (rnd_position + 1) % rnd_size] % (height - y + neighborRange - 1);
		}
	}
	return image_data[dx + dy * stride];
}

int LibVibeModelInit(VibeModel *model, const unsigned char *image_data, const unsigned int width, const unsigned int height, const unsigned int stride)
{
	if (!model || !image_data || !width || !height || !stride || (stride < width)) return 1;
	model->width = width;
	model->height = height;
	model->stride = stride;
	model->ptr_pixels = 0;
	model->ptr_pixels = (Pixel*)calloc(model->width*model->height, sizeof(Pixel));
	if (!model->ptr_pixels) return 1;
	for (unsigned int i = 0; i < model->width*model->height; i++)
	{
		model->ptr_pixels[i].number_of_samples = model->number_of_samples;
		model->ptr_pixels[i].size_of_sample = 1;
		model->ptr_pixels[i].ptr_samples = 0;
		model->ptr_pixels[i].ptr_samples = (unsigned char*)calloc(model->number_of_samples, sizeof(unsigned char));
		if (!model->ptr_pixels[i].ptr_samples) return 1;
	}
	unsigned int n = 0;
	for (unsigned int j = 0; j < model->height; j++)
	{
		for (unsigned int i = 0; i < model->width; i++)
		{
			model->ptr_pixels[n].ptr_samples[0] = image_data[i + j * stride];
			for (unsigned int k = 1; k < model->number_of_samples; k++)
				model->ptr_pixels[n].ptr_samples[k] = get_rand_pixel(image_data, width, height, stride, i, j);
			n++;
		}
	}
	return 0;
}

int LibVibeModelUpdate(VibeModel *model, const unsigned char *image_data, unsigned char *segmentation_map)
{
	int ad = model->stride - model->width;

	if (!model || !image_data || !segmentation_map) return 1;
	if (model->stride < model->width) return 1;

	unsigned int n = 0;
	for (int j = 0; j < model->height; j++)
	{
		for (int i = 0; i < model->width; i++)
		{
			bool flag = false;
			unsigned int matchingCounter = 0;
			for (unsigned int t = 0; t < model->ptr_pixels[n].number_of_samples; t++)
			{
				if (abs((int)image_data[n] - (int)model->ptr_pixels[n].ptr_samples[t]) < model->matching_threshold)
				{

					matchingCounter++;
					if (matchingCounter >= model->matching_number)
					{
						flag = true;
						break;
					}
				}
			}

			if (flag)
			{

				segmentation_map[n] = 0;
				if (!(ptr_rnd[rnd_position = (rnd_position + 1) % rnd_size] % model->update_factor))
				{
					model->ptr_pixels[i + model->width*j].ptr_samples[ptr_rnd[rnd_position = (rnd_position + 1) % rnd_size] % model->number_of_samples] = image_data[n];

					unsigned int m = (model->stride * j + i);
					switch ((ptr_rnd[rnd_position = (rnd_position + 1) % rnd_size]) % 8)
					{
					case 0:
						if ((model->width - 1) <= i)
						{
						}
						else
						{
							m++;
						}
						break;
					case 1:
						if ((model->width - 1) <= i)
						{
						}
						else
						{
							m++;
						}
						if ((model->height - 1) <= j)
						{
						}
						else
						{
							m += model->stride;
						}
						break;
					case 2:
						if ((model->height - 1) <= j)
						{
						}
						else
						{
							m += model->stride;
						}
						break;
					case 3:
						if (i <= 0)
						{
						}
						else
						{
							m--;
						}
						if ((model->height - 1) <= j)
						{
						}
						else
						{
							m += model->stride;
						}
						break;
					case 4:
						if (i <= 0)
						{
						}
						else
						{
							m--;
						}
						break;
					case 5:
						if (i <= 0)
						{
						}
						else
						{
							m--;
						}
						if (j <= 0)
						{
						}
						else
						{
							m -= model->stride;
						}
						break;
					case 6:
						if (j <= 0)
						{
						}
						else
						{
							m -= model->stride;
						}
						break;
					case 7:
						if ((model->width - 1) <= i)
						{
						}
						else
						{
							m++;
						}
						if (j <= 0)
						{
						}
						else
						{
							m -= model->stride;
						}
						break;
					default:
						puts("You should not see this message!!!");
						break;
					}
					model->ptr_pixels[m].ptr_samples[ptr_rnd[rnd_position = (rnd_position + 1) % rnd_size] % model->number_of_samples] = image_data[n];
				}
			}
			else
			{
				segmentation_map[n] = 255;
			}
			n++;
		}
		if (model->stride > model->width)
			n += ad;
	}
	return 0;
}


