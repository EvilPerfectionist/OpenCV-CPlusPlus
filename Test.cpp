#include <iostream>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  

using namespace cv;

int main() {
	IplImage* src = cvLoadImage("Test.png");
	CvSize pImgSize;
	pImgSize = cvGetSize(src);
	IplImage* hsv = cvCreateImage(pImgSize, 8, 3);
	IplImage* h_plane = cvCreateImage(pImgSize, 8, 1);
	IplImage* s_plane = cvCreateImage(pImgSize, 8, 1);
	IplImage* v_plane = cvCreateImage(pImgSize, 8, 1);
	cvSplit(src, h_plane, s_plane, v_plane, 0);
	cvShowImage("Color-h", h_plane);
	cvShowImage("Color-s", s_plane);
	cvShowImage("Color-v", v_plane);
	cvShowImage("src", src);
	waitKey(6000);
}