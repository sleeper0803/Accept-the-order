#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include "STCTracker.h"


// Global variables
Rect box;
Rect boxRegion;//STC algorithm display context region 

// bounding box mouse callback
void MouseHandler(int event, int x, int y, int flags, void *param){

	const cv::Mat &src = *static_cast<cv::Mat*>(param);
	char text[100];

	static cv::Point pre_pt(-1, -1);//初始坐标  
	static cv::Point cur_pt(-1, -1);//实时坐标  

	Mat tmpImg;
	Mat roiImg;
	if (param==NULL)
	{
		return;
	}
	switch( event ){
		case CV_EVENT_LBUTTONDOWN:
			src.copyTo(tmpImg);//将原始图片复制到img中  		 
			sprintf_s(text, "(%d,%d)", x, y);
			pre_pt = Point(x, y);
			putText(tmpImg, text, pre_pt, FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 0, 255), 1, 8);//在窗口上显示坐标  
			circle(tmpImg, pre_pt, 2, Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);//划圆  
			imshow("Tracker", tmpImg);
			box = Rect(x, y, 0, 0);
			break;
		case CV_EVENT_MOUSEMOVE:
			if (flags & CV_EVENT_FLAG_LBUTTON){
				src.copyTo(tmpImg);//将原始图片复制到img中  
				sprintf_s(text, "(%d,%d)", x, y);
				cur_pt = Point(x, y);
				putText(tmpImg, text, cur_pt, FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 0, 255));
				rectangle(tmpImg, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//在临时图像上实时显示鼠标拖动时形成的矩形  
			}
			if (flags != CV_EVENT_FLAG_LBUTTON){				
				if (roiImg.data == NULL){
					src.copyTo(tmpImg);
				}
				else{
					roiImg.copyTo(tmpImg);
				}
				sprintf_s(text, "(%d,%d)", x, y);
				cv::Point temp_pt = Point(x, y);
				putText(tmpImg, text, temp_pt, FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 0, 255));
				rectangle(tmpImg, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//在临时图像上实时显示鼠标拖动时形成的矩形  
			}
			imshow("Tracker", tmpImg);
			break;
		case CV_EVENT_LBUTTONUP:
			src.copyTo(roiImg);//将原始图片复制到img中  
			sprintf_s(text, "(%d,%d)", x, y);
			cur_pt = Point(x, y);
			putText(roiImg, text, cur_pt, FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 0, 255));
			rectangle(roiImg, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//根据初始点和结束点，将矩形画到src上 
			imshow("Tracker", roiImg);

			//截取矩形包围的图像，并保存到dst中  
			box.x = min(cur_pt.x, pre_pt.x);
			box.y = min(cur_pt.y, pre_pt.y);
			box.width = abs(pre_pt.x - cur_pt.x);
			box.height = abs(pre_pt.y - cur_pt.y);
			break;
		default:
			;			
	}
}

int main()
{
	VideoCapture CCapture;
	CCapture.open("3.avi");
	Mat Frame;
	if (!CCapture.isOpened())
	{
		cout<<"capture device failed to open!"<<endl;
		return -1;
	}
	unsigned int total_frame_number = CCapture.get(CV_CAP_PROP_FRAME_COUNT);
	cout<<"TotalFrameNumber="<<CCapture.get(CV_CAP_PROP_FRAME_COUNT)<<endl;
	
	Rect STCbox;
	STCTracker stcTracker;

	int current_frame = 1;
	while (current_frame<total_frame_number)
	{
		CCapture >> Frame;
		if (current_frame == 1){
			//Register mouse callback to draw the bounding box
			cvNamedWindow("Tracker", CV_WINDOW_AUTOSIZE);
			imshow("Tracker", Frame);
			cvSetMouseCallback("Tracker", MouseHandler, static_cast<void*>(&Frame));
		
			cv::waitKey(0);
			destroyWindow("Tracker"); //关闭此窗口，鼠标监控函数不可用
			STCbox = box;
			stcTracker.init(Frame, STCbox, boxRegion);
			current_frame++;
			continue;
		}
		stcTracker.tracking(Frame, STCbox,boxRegion, current_frame);
		stringstream buf;
		buf << current_frame;
		string num = buf.str();
		putText(Frame, num, Point(15, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 3);
		rectangle(Frame, Point(STCbox.x, STCbox.y), Point(STCbox.x + STCbox.width, STCbox.y + STCbox.height), Scalar(0, 255, 0, 0), 1, 8, 0);//根据初始点和结束点，将矩形画到src上 
		imshow("Tracker", Frame);
		cv::waitKey(1);
		current_frame++;
	}
	return 0;
}