#include <iostream>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <cmath>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	/*CvCapture* capture = cvCreateCameraCapture( 0 );*/

	CvSize size;
	size.width = 768;
	size.height = 576;
	IplImage * src = cvCreateImageHeader(size, IPL_DEPTH_8U, 3);

	/*cvNamedWindow( "H-S Histogram", 1 );*/

	IplImage* hsv = cvCreateImage(cvGetSize(src), 8, 3);
	IplImage* h_plane = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* s_plane = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* v_plane = cvCreateImage(cvGetSize(src), 8, 1);

	IplImage* mapx = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
	IplImage* mapy = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
	
	src = cvLoadImage("1.jpg");

									  /** 输入图像转换到HSV颜色空间 */
									  /*cvCvtColor( src, hsv, CV_BGR2HSV );*/
	cvSplit(src, h_plane, s_plane, v_plane, 0);
	/*cvFlip(h_plane,h_plane);
	cvFlip(s_plane,s_plane);
	cvFlip(v_plane,v_plane);*/

	cvShowImage("Color-h", h_plane);
	cvShowImage("Color-s", s_plane);
	cvShowImage("Color-v", v_plane);
	cvShowImage("src", src);

	/*		cvShowImage( "H-S Histogram", hist_img );*/

	int c = cvWaitKey(0);
	if (c == 'p') {
		c = 0;
		while (c != 'p' && c != 27) {
			c = cvWaitKey(250);
		}
	}
	if (c == 27) {
		return 0;
	}

	/*src = cvQueryFrame(capture);*/
	/*}*/
	return 0;
}