#ifndef _BACKGROUND_SUBTRACT_H_
#define _BACKGROUND_SUBTRACT_H_

#include "VIBE.h"

class BackgroundSubtract
{
public:
	BackgroundSubtract();
	~BackgroundSubtract();
	void Init(cv::Mat &image);
	void Subtract(const cv::Mat &image, cv::Mat &foreground);
private:
	VibeModel *CModel;

};

#endif
