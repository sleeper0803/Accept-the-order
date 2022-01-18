#include "background_subtract.h"

BackgroundSubtract::BackgroundSubtract()
{
	CModel = (VibeModel*)calloc(1, sizeof(VibeModel));

	CModel->number_of_samples = 20;
	CModel->matching_threshold = 20;
	CModel->matching_number = 2;
	CModel->update_factor =5;//5£¬16
	InitRnd(256);
}

BackgroundSubtract::~BackgroundSubtract()
{
	for (unsigned int i = 0; i < CModel->width*CModel->height; i++)
	{
		free(CModel->ptr_pixels[i].ptr_samples);
	}
	free(CModel->ptr_pixels);
	freeRnd();
}

void BackgroundSubtract::Init(cv::Mat &image)
{
	int32_t width = image.size().width;
	int32_t height = image.size().height;
	int32_t stride = image.channels()*image.size().width;
	uint8_t *image_data = (uint8_t*)image.data;

	LibVibeModelInit(CModel, image_data, width, height, stride);
}

void BackgroundSubtract::Subtract(const cv::Mat &image, cv::Mat &foreground)
{
	uint8_t *image_data = (uint8_t*)image.data;
	uint8_t *segmentation_map = (uint8_t*)foreground.data;
	//cv::Mat erodeElement = cv::getStructuringElement(0, cv::Size(2, 2), cv::Point(-1, -1));
	//cv::Mat dilateElement = cv::getStructuringElement(0, cv::Size(2, 2), cv::Point(-1, -1));

	LibVibeModelUpdate(CModel, image_data, segmentation_map);

	/*cv::erode(foreground, foreground, erodeElement, cv::Point(-1, -1), 1);
	cv::dilate(foreground, foreground, dilateElement, cv::Point(-1,-1), 2);*/
}
