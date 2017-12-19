#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <cmath>
#include "cxcore.h"

int OutputWidth = 768;
int OutputHeight = 576;										
#define MAX_CLUSTERS 4										//类别数
#define SAMPLE_NUMBER 10000									//样本数 
int hold = 0.5;


/***************************
*	计算LBP等声明函数模块  *
***************************/
void ComputeLBP(IplImage *img, IplImage *LBPImage) {
	IplImage* GrayImage = cvCreateImage(cvGetSize(img), 8, 1);
	cvCvtColor(img, GrayImage, CV_BGR2GRAY);
	int center = 0;
	int center_lbp = 0;
	for (int row = 1; row < GrayImage->height - 1; row++) {
		for (int col = 1; col < GrayImage->width - 1; col++) {
			center = cvGetReal2D(GrayImage, row, col);
			center_lbp = 0;
			if (center <= cvGetReal2D(GrayImage, row - 1, col - 1))
				center_lbp += 1;
			if (center <= cvGetReal2D(GrayImage, row - 1, col))
				center_lbp += 2;
			if (center <= cvGetReal2D(GrayImage, row - 1, col + 1))
				center_lbp += 4;
			if (center <= cvGetReal2D(GrayImage, row, col - 1))
				center_lbp += 8;
			if (center <= cvGetReal2D(GrayImage, row, col + 1))
				center_lbp += 16;
			if (center <= cvGetReal2D(GrayImage, row + 1, col - 1))
				center_lbp += 32;
			if (center <= cvGetReal2D(GrayImage, row + 1, col))
				center_lbp += 64;
			if (center <= cvGetReal2D(GrayImage, row + 1, col + 1))
				center_lbp += 128;
			cvSetReal2D(LBPImage, row, col, center_lbp);
		}
	}
}

void ComputeLTP(IplImage *img, IplImage *LTPImage_p, IplImage *LTPImage_n, int K) {
	IplImage* GrayImage = cvCreateImage(cvGetSize(img), 8, 1);
	cvCvtColor(img, GrayImage, CV_BGR2GRAY);
	int center = 0;
	int center_ltp_p = 0;
	int center_ltp_n = 0;
	for (int row = 1; row < GrayImage->height - 1; row++) {
		for (int col = 1; col < GrayImage->width - 1; col++) {
			center = cvGetReal2D(GrayImage, row, col);
			center_ltp_n = 0;
			if (center >= cvGetReal2D(GrayImage, row - 1, col - 1) + K)
				center_ltp_n += 1;
			if (center >= cvGetReal2D(GrayImage, row - 1, col) + K)
				center_ltp_n += 2;
			if (center >= cvGetReal2D(GrayImage, row - 1, col + 1) + K)
				center_ltp_n += 4;
			if (center >= cvGetReal2D(GrayImage, row, col - 1) + K)
				center_ltp_n += 8;
			if (center >= cvGetReal2D(GrayImage, row, col + 1) + K)
				center_ltp_n += 16;
			if (center >= cvGetReal2D(GrayImage, row + 1, col - 1) + K)
				center_ltp_n += 32;
			if (center >= cvGetReal2D(GrayImage, row + 1, col) + K)
				center_ltp_n += 64;
			if (center >= cvGetReal2D(GrayImage, row + 1, col + 1) + K)
				center_ltp_n += 128;
			cvSetReal2D(LTPImage_n, row, col, center_ltp_n);//正值矩阵输入

			center_ltp_p = 0;
			if (center <= cvGetReal2D(GrayImage, row - 1, col - 1) - K)
				center_ltp_p += 1;
			if (center <= cvGetReal2D(GrayImage, row - 1, col) - K)
				center_ltp_p += 2;
			if (center <= cvGetReal2D(GrayImage, row - 1, col + 1) - K)
				center_ltp_p += 4;
			if (center <= cvGetReal2D(GrayImage, row, col - 1) - K)
				center_ltp_p += 8;
			if (center <= cvGetReal2D(GrayImage, row, col + 1) - K)
				center_ltp_p += 16;
			if (center <= cvGetReal2D(GrayImage, row + 1, col - 1) - K)
				center_ltp_p += 32;
			if (center <= cvGetReal2D(GrayImage, row + 1, col) - K)
				center_ltp_p += 64;
			if (center <= cvGetReal2D(GrayImage, row + 1, col + 1) - K)
				center_ltp_p += 128;
			cvSetReal2D(LTPImage_p, row, col, center_ltp_p);//负值矩阵输入
		}
	}
}
void ComputeLATP(IplImage *img, IplImage *LATPImage_p, IplImage *LATPImage_n, int K) {
	IplImage* GrayImage = cvCreateImage(cvGetSize(img), 8, 1);
	cvCvtColor(img, GrayImage, CV_BGR2GRAY);
	int center = 0;
	int center_latp_p = 0;
	int center_latp_n = 0;
	int c1 = 0;
	int c2 = 0;
	int c3 = 0;
	int c4 = 0;
	int c5 = 0;
	int c6 = 0;
	int c7 = 0;
	int c8 = 0;
	double u = 0;
	double q = 0;
	for (int row = 1; row < GrayImage->height - 1; row++) {
		for (int col = 1; col < GrayImage->width - 1; col++)
		{
			center = cvGetReal2D(GrayImage, row, col);
			c1 = cvGetReal2D(GrayImage, row - 1, col - 1);
			c2 = cvGetReal2D(GrayImage, row - 1, col);
			c3 = cvGetReal2D(GrayImage, row - 1, col + 1);
			c4 = cvGetReal2D(GrayImage, row, col - 1);
			c5 = cvGetReal2D(GrayImage, row, col + 1);
			c6 = cvGetReal2D(GrayImage, row + 1, col - 1);
			c7 = cvGetReal2D(GrayImage, row + 1, col);
			c8 = cvGetReal2D(GrayImage, row + 1, col + 1);

			u = (c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8) / 8;
			q = sqrt((c1 - u)*(c1 - u) + (c2 - u)*(c2 - u) + (c3 - u)*(c3 - u) + (c4 - u)*(c4 - u) + (c5 - u)*(c5 - u) + (c6 - u)*(c6 - u) + (c7 - u)*(c7 - u) + (c8 - u)*(c8 - u));
			center = (int)u - K*(int)q;
			center_latp_p = 0;
			if (center >= cvGetReal2D(GrayImage, row - 1, col - 1))
				center_latp_p += 1;
			if (center >= cvGetReal2D(GrayImage, row - 1, col))
				center_latp_p += 2;
			if (center >= cvGetReal2D(GrayImage, row - 1, col + 1))
				center_latp_p += 4;
			if (center >= cvGetReal2D(GrayImage, row, col - 1))
				center_latp_p += 8;
			if (center >= cvGetReal2D(GrayImage, row, col + 1))
				center_latp_p += 16;
			if (center >= cvGetReal2D(GrayImage, row + 1, col - 1))
				center_latp_p += 32;
			if (center >= cvGetReal2D(GrayImage, row + 1, col))
				center_latp_p += 64;
			if (center >= cvGetReal2D(GrayImage, row + 1, col + 1))
				center_latp_p += 128;
			cvSetReal2D(LATPImage_p, row, col, center_latp_p);//正值矩阵输入

			center = (int)u + K*(int)q;
			center_latp_n = 0;
			if (center <= cvGetReal2D(GrayImage, row - 1, col - 1))
				center_latp_n += 1;
			if (center <= cvGetReal2D(GrayImage, row - 1, col))
				center_latp_n += 2;
			if (center <= cvGetReal2D(GrayImage, row - 1, col + 1))
				center_latp_n += 4;
			if (center <= cvGetReal2D(GrayImage, row, col - 1))
				center_latp_n += 8;
			if (center <= cvGetReal2D(GrayImage, row, col + 1))
				center_latp_n += 16;
			if (center <= cvGetReal2D(GrayImage, row + 1, col - 1))
				center_latp_n += 32;
			if (center <= cvGetReal2D(GrayImage, row + 1, col))
				center_latp_n += 64;
			if (center <= cvGetReal2D(GrayImage, row + 1, col + 1))
				center_latp_n += 128;
			cvSetReal2D(LATPImage_n, row, col, center_latp_n);//负值矩阵输入
		}
	}
}



int main(int argc, char** argv) {


	CvSize size;
	size.width = 768;
	size.height = 576;
	IplImage * img = cvCreateImageHeader(size, IPL_DEPTH_8U, 3);

	int value = 0;

	IplImage* LBPImage = cvCreateImage(size, 8, 1);
	IplImage* GrayImage = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage *LTPImage_p = cvCreateImage(size, 8, 1);

	IplImage *LTPImage_n = cvCreateImage(size, 8, 1);


	cvNamedWindow("img", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("LBP", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("LTP_p", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("LTP_n", CV_WINDOW_AUTOSIZE);

	while (1)
	{
		img = cvLoadImage("1.jpg");

		IplImage *t = cvCloneImage(img);										

		ComputeLBP(img, LBPImage);
		ComputeLTP(img, LTPImage_p, LTPImage_n, 1);


		cvCvtColor(img, GrayImage, CV_BGR2GRAY);

		
		cvShowImage("img", img);
		cvShowImage("LBP", LBPImage);
		cvShowImage("LTP_p", LTPImage_p);
		cvShowImage("LTP_n", LTPImage_n);

		cvReleaseImage(&t);
		int c = cvWaitKey(33);
		if (c == 'p') {
			c = 0;
			while (c != 'p' && c != 27) {
				c = cvWaitKey(250);
			}
		}
		if (c == 27) {
			return 0;
		}
		
		cvZero(img);
		/*img = cvQueryFrame(capture);*/
	}

	cvWaitKey(0);
	cvReleaseImage(&img);
	cvReleaseImage(&LBPImage);
	cvReleaseImage(&LTPImage_p);
	cvReleaseImage(&LTPImage_n);
	cvDestroyAllWindows();

	return 0;

}
