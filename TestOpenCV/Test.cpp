#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <cmath>
#include "cxcore.h"
#include <stdio.h>
#include "svm.h"

int OutputWidth = 768;
int OutputHeight = 576;
#define MAX_CLUSTERS 2										//类别数
#define SAMPLE_NUMBER 10000									//样本数 
#define PI 3.14159265
int hold = 2;												//LTP阈值
int kernel = 3;												//LTP直方图 n*n算子

void ComputeLTP(IplImage *GrayImage, int i, int j, float s[8], int binp[8], int bino[8], int binn[8]) {
	int u;
	double q = 0;
	int   c[8];
	int p;
	for (int m = 0; m< kernel; m++) {
		for (int n = 0; n< kernel; n++) {
			u = cvGetReal2D(GrayImage, j + 3 * (2 * n - kernel + 1) / 2, i + 3 * (2 * m - kernel + 1) / 2);
			c[0] = cvGetReal2D(GrayImage, j + 3 * (2 * n - kernel + 1) / 2 - 1, i + 3 * (2 * m - kernel + 1) / 2 - 1);
			c[1] = cvGetReal2D(GrayImage, j + 3 * (2 * n - kernel + 1) / 2 - 1, i + 3 * (2 * m - kernel + 1) / 2);
			c[2] = cvGetReal2D(GrayImage, j + 3 * (2 * n - kernel + 1) / 2 - 1, i + 3 * (2 * m - kernel + 1) / 2 + 1);
			c[3] = cvGetReal2D(GrayImage, j + 3 * (2 * n - kernel + 1) / 2, i + 3 * (2 * m - kernel + 1) / 2 + 1);
			c[4] = cvGetReal2D(GrayImage, j + 3 * (2 * n - kernel + 1) / 2 + 1, i + 3 * (2 * m - kernel + 1) / 2 + 1);
			c[5] = cvGetReal2D(GrayImage, j + 3 * (2 * n - kernel + 1) / 2 + 1, i + 3 * (2 * m - kernel + 1) / 2);
			c[6] = cvGetReal2D(GrayImage, j + 3 * (2 * n - kernel + 1) / 2 + 1, i + 3 * (2 * m - kernel + 1) / 2 - 1);
			c[7] = cvGetReal2D(GrayImage, j + 3 * (2 * n - kernel + 1) / 2, i + 3 * (2 * m - kernel + 1) / 2 - 1);
		
			for (p = 0; p < 8; p++) {
				if (u <= (c[p] - hold))
					binp[p]++;
				else if (u >= (c[p] + hold))
					binn[p]++;
				else
					bino[p]++;
			}
		}
	}
	for (p = 0; p < 8; p++) {
		if ((binp[p] >= bino[p]) && (binp[p] >= binn[p]))
			s[p] = 0.6;
		else if ((bino[p] > binp[p]) && (bino[p] > binn[p]))
			s[p] = 0.3;
		else
			s[p] = 0;
	}
}
void load(int n, float w[1000], float x[1000][14]) {
	FILE  *fp;
	int i = 0;

	fp = fopen("SV_coef.txt", "rb");
	for (i = 0; i < n; i++) {
		fscanf(fp, "%f", &w[i]);
		/*printf( "%f\n", w[i]);*/
	}
	fclose(fp);

	fp = fopen("SVs.txt", "rb");
	for (i = 0; i < n; i++) {
		for (int j = 0; j < 14; j++)
		{
			fscanf(fp, "%f", &x[i][j]);
			/*printf( "%f ", x[i][j]);*/
		}
		/*printf( "\n" );*/
	}
	fclose(fp);
}

int main(int argc, char* argv[])
{
	/*Practise;*/
	CvSize size;
	size.width = OutputWidth;
	size.height = OutputHeight;
	IplImage* img = cvCreateImageHeader(size, IPL_DEPTH_8U, 3);
	IplImage* hsv = cvCreateImage(size, IPL_DEPTH_8U, 3);
	IplImage* GrayImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	int sample_count = SAMPLE_NUMBER;

	CvScalar color_tab[5];
	color_tab[0] = CV_RGB(255, 0, 0);
	color_tab[1] = CV_RGB(0, 255, 0);
	color_tab[2] = CV_RGB(255, 0, 255);
	color_tab[3] = CV_RGB(255, 255, 0);
	color_tab[4] = CV_RGB(0, 255, 255);
	CvMat* clusters = cvCreateMat(sample_count, 1, CV_32SC1);

	float b = 0;
	float gamma = 0;
	int n = 0;
	float w[1000] = { 0 };
	float x[1000][14] = { 0 };

	FILE* fp;

	fp = fopen("Parameters.txt", "rb");
	fscanf(fp, "%f", &b);
	fscanf(fp, "%d", &n);
	fscanf(fp, "%f", &gamma);
	fclose(fp);
	load(n, w, x);
	printf("%f\n", b);
	printf("%d\n", n);
	printf("%f\n", gamma);

	cvNamedWindow("SVM-Result", CV_WINDOW_AUTOSIZE);

	while (1) {

		img = cvLoadImage("38.jpg");

		IplImage* src = cvCloneImage(img);
		IplImage* study = cvCloneImage(img);

		cvCvtColor(img, hsv, CV_BGR2HSV);
		cvCvtColor(img, GrayImage, CV_BGR2GRAY);
		float range = 0;
		float weight = 0;
		int plabel = -1;

		/*int k = 0;*/
		for (int i = 0; i < OutputWidth; i = i + 3) {
			for (int j = 0; j < OutputHeight; j = j + 3) {
				if ((i > 3 * (kernel - 1) / 2) && (j > 3 * (kernel - 1) / 2) && (i < OutputWidth - 1 - 3 * (kernel - 1) / 2) && (j < OutputHeight - 1 - 3 * (kernel - 1) / 2)) {
					float e[14];
					e[0] = (float)cvGet2D(hsv, j, i).val[0] / 180;			//HSV三通道
					e[1] = (float)cvGet2D(hsv, j, i).val[1] / 256;
					e[2] = (float)cvGet2D(hsv, j, i).val[2] / 256;
					e[3] = (float)cvGet2D(img, j, i).val[0] / 256;			//RGB三通道
					e[4] = (float)cvGet2D(img, j, i).val[1] / 256;
					e[5] = (float)cvGet2D(img, j, i).val[2] / 256;

					int binp[8] = { 0 }, bino[8] = { 0 }, binn[8] = { 0 };
					float s[8] = { 0 };
					ComputeLTP(GrayImage, i, j, s, binp, bino, binn);
					for (int p = 0; p < 8; p++) {
						e[p + 6] = s[p];
					}
					for (int k = 0; k < n; k++) {
						for (int p = 0; p < 14; p++) {
							range = (x[k][p] - e[p])*(x[k][p] - e[p]) + range;
						}
						weight = exp(-gamma * range)* w[k] + weight;
						range = 0;
					}
					weight = weight + b;
					if (weight >= 0) {
						plabel = 0;
					}
					else {
						plabel = 1;
					}
					weight = 0;

					CvPoint2D32f pt;
					pt.x = i;
					pt.y = j;
					cvCircle(img, cvPointFrom32f(pt), 1, color_tab[plabel], CV_FILLED);

				}
			}
		}

		cvShowImage("SVM-Result", img);
		IplImage* flood1 = cvCloneImage(img);
		IplImage* flood2 = cvCloneImage(img);
		IplImage* pMask = cvCreateImage(cvSize(img->width + 2, img->height + 2), IPL_DEPTH_8U, 1);


		cvNamedWindow("mask1", CV_WINDOW_AUTOSIZE);
		cvNamedWindow("mask2", CV_WINDOW_AUTOSIZE);


		cvSetZero(pMask);

		cvFloodFill(																				//漫水填充形态学处理
			flood1,
			cvPoint(OutputWidth / 2, OutputHeight - 10),
			CV_RGB(255, 255, 0),
			cvScalar(30, 30, 40, 0),
			cvScalar(5, 30, 40, 0),
			NULL,
			8 | CV_FLOODFILL_FIXED_RANGE | (0x9f << 8),
			pMask
		);
		cvShowImage("mask1", pMask);
		cvMorphologyEx(pMask, pMask, NULL, NULL, CV_MOP_CLOSE);
		IplImage* pContourImg = cvCreateImage(cvGetSize(pMask), IPL_DEPTH_8U, 3);					//检测边缘
		CvMemStorage* storage = 0;
		storage = cvCreateMemStorage(0);
		CvSeq* contour = 0;
		CvSeq* contmax = 0;
		cvCvtColor(pMask, pContourImg, CV_GRAY2BGR);
		cvFindContours(pMask, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));              // 查找外边缘
		cvDrawContours(src, contour, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), 2, -1, 8, cvPoint(0, 0));
		/*CvRect aRect = cvBoundingRect( contmax, 0 );
		cvSetImageROI( pContourImg,aRect);*/
		cvShowImage("mask2", pContourImg);
		cvShowImage("Source", src);
		cvNamedWindow("Source1", CV_WINDOW_AUTOSIZE);
		cvShowImage("Source1", flood1);

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
	}
	return 0;
}