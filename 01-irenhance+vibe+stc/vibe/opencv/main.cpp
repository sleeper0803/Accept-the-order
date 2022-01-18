#include"vibe.h"
#include"background_subtract.h"
#include<opencv2\opencv.hpp>

#define USE_VIBE
int main(){
	cv::VideoCapture CCapLoad;
	CCapLoad.open("3.avi");

	int total_frames = CCapLoad.get(CV_CAP_PROP_FRAME_COUNT);
	const unsigned int kIrImgWidth = CCapLoad.get(CV_CAP_PROP_FRAME_WIDTH);
	const unsigned int kIrImgHeight = CCapLoad.get(CV_CAP_PROP_FRAME_HEIGHT);

	cv::Mat CInputImage(kIrImgHeight, kIrImgWidth, CV_8UC3);
	cv::Mat CImageDenoising(kIrImgHeight, kIrImgWidth, CV_8UC3);
	cv::Mat CImageEnhance(kIrImgHeight, kIrImgWidth, CV_8UC1);
	cv::Mat CForeground(kIrImgHeight, kIrImgWidth,CV_8UC1);

	BackgroundSubtract CSubtractor;
	unsigned int current_frame = 1;

	while (current_frame <=	total_frames){
		CCapLoad >> CInputImage;
		//step1:Ωµ‘Î‘ˆ«ø
		//https://setosa.io/ev/image-kernels/
		//cv::Mat kernel = (cv::Mat_<float>(3, 3) << -1, -1, -1, -1, 8, -1, -1, -1, -1);//outline
		cv::Mat CKernel = (cv::Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0); //sharpen
		cv::cvtColor(CInputImage, CInputImage, CV_BGR2GRAY);
		medianBlur(CInputImage, CImageDenoising, (3, 3));
		filter2D(CImageDenoising, CImageEnhance, CV_8UC1, CKernel);
		cv::imshow("InputImage", CInputImage);
		//cv::imshow("CImageDenoising", CImageDenoising);
		//cv::imshow("CImageEnhance", CImageEnhance);
		cv::waitKey(1);

#ifdef USE_VIBE
		//step2:Vibe
		//https://blog.csdn.net/guchensha/article/details/98497032
		//https://www.shangmayuan.com/a/f6b07318fd1b4112940b5b00.html
		//http://www.4k8k.xyz/article/u010712012/85644117
		if (current_frame == 1) {
			current_frame++;
			CSubtractor.Init(CInputImage);
			continue;
		}
		CSubtractor.Subtract(CInputImage, CForeground);
		cv::imshow("Foreground", CForeground);
		cv::waitKey(1);
#endif // VIBE
		current_frame++;
	}
	return 0;
}