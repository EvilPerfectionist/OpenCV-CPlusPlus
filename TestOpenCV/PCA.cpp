// PCA降维处理.cpp : 定义控制台应用程序的入口点。
//

#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <cmath>
#include "cxcore.h"

int OutputWidth  = 768;
int OutputHeight = 576; 									
#define MAX_CLUSTERS 4										//类别数
#define SAMPLE_NUMBER 100									//样本数 
int hold = 2;
int kernel = 3;	

void ComputeLTP(IplImage *GrayImage, int i, int j,float s[8], int binp[8],int bino[8],int binn[8]){
	int u ;
	double q = 0;
	int   c[8];
	int p;
	for (int m = 0; m< kernel; m++){
		for (int n = 0; n< kernel; n++){
			u    = cvGetReal2D(GrayImage, j+3*(2*n-kernel+1)/2  , i+3*(2*m-kernel+1)/2  );
			c[0] = cvGetReal2D(GrayImage, j+3*(2*n-kernel+1)/2-1, i+3*(2*m-kernel+1)/2-1);
			c[1] = cvGetReal2D(GrayImage, j+3*(2*n-kernel+1)/2-1, i+3*(2*m-kernel+1)/2  );
			c[2] = cvGetReal2D(GrayImage, j+3*(2*n-kernel+1)/2-1, i+3*(2*m-kernel+1)/2+1);
			c[3] = cvGetReal2D(GrayImage, j+3*(2*n-kernel+1)/2  , i+3*(2*m-kernel+1)/2+1);
			c[4] = cvGetReal2D(GrayImage, j+3*(2*n-kernel+1)/2+1, i+3*(2*m-kernel+1)/2+1);
			c[5] = cvGetReal2D(GrayImage, j+3*(2*n-kernel+1)/2+1, i+3*(2*m-kernel+1)/2  );
			c[6] = cvGetReal2D(GrayImage, j+3*(2*n-kernel+1)/2+1, i+3*(2*m-kernel+1)/2-1);
			c[7] = cvGetReal2D(GrayImage, j+3*(2*n-kernel+1)/2  , i+3*(2*m-kernel+1)/2-1);
			//for ( p = 0; p < 8; p++){
			//	u = u + c[p];
			//	q = q + c[p]*c[p];
			//}
			//u = u/8;
			//q = sqrt(q);
			for ( p = 0; p < 8; p++){
				if		( u <= (c[p] - hold ))
					binp[p]++;
				else if ( u >= (c[p] + hold ))
					binn[p]++;
				else
					bino[p]++;
			}
		}
	}
	for ( p = 0; p < 8; p++){
		if ( (binp[p] >= bino[p])&&(binp[p] >= binn[p]))
			s[p] = 1;
		else if ( (bino[p] > binp[p])&&(bino[p] > binn[p]))
			s[p] = 0.5;
		else 
			s[p] = 0;
	}
}
void PrintMatrix(CvMat *Matrix,int Rows,int Cols)
{
	for(int i=0;i<Rows;i++)
	{
		for(int j=0;j<Cols;j++)
		{
			printf("%.2f ",cvGet2D(Matrix,i,j).val[0]);
		}
		printf("\n");
	}
}
int main(int argc, char* argv[])
{
	CvSize size;
	size.width  = OutputWidth;
	size.height = OutputHeight;
	IplImage* img = cvCreateImageHeader(size,IPL_DEPTH_8U,3);
	IplImage* hsv = cvCreateImage( size, IPL_DEPTH_8U, 3);
	IplImage* GrayImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	//CvMat * mat = cvCreateMat(img->height, img->width, CV_64FC3);

	CvRNG rng = CvRNG( cvGetTickCount() );

	cvNamedWindow("clusters",CV_WINDOW_AUTOSIZE);

	int k, p, cluster_count = MAX_CLUSTERS;
	int i, j,  sample_count = SAMPLE_NUMBER;
	float s[8] = {0};
	CvMat* points   = cvCreateMat( sample_count,14, CV_32FC1);					//数据样本
	CvMat* points2  = cvCreateMat( sample_count, 1, CV_32FC2);					//储存坐标信息

	while(1){
		
		img = cvLoadImage("1.jpg");
		IplImage *t = cvCloneImage(img);								
		cvCvtColor(img, hsv, CV_BGR2HSV);
		cvCvtColor(img, GrayImage, CV_BGR2GRAY);
		/*多元高斯分布生成随机样本*/
		k=0;
		while (k < sample_count){												//特征提取
			i = cvRandInt(&rng)%img->width;
			j = cvRandInt(&rng)%img->height;
			if (( i > 3*(kernel-1)/2) && ( j > 3*(kernel-1)/2) && ( i < 767-3*(kernel-1)/2) && ( j < 575-3*(kernel-1)/2)){
				((CvPoint2D32f*)points2->data.fl)[k].x = i;
				((CvPoint2D32f*)points2->data.fl)[k].y = j;

				CvScalar e[14];
				e[0].val[0]=(float)cvGet2D(hsv, j, i).val[0]/180;			//HSV三通道
				e[1].val[0]=(float)cvGet2D(hsv, j, i).val[1]/256;
				e[2].val[0]=(float)cvGet2D(hsv, j, i).val[2]/256;
				e[3].val[0]=(float)cvGet2D(img, j, i).val[0]/256;			//RGB三通道
				e[4].val[0]=(float)cvGet2D(img, j, i).val[1]/256;
				e[5].val[0]=(float)cvGet2D(img, j, i).val[2]/256;

				int binp[8] = {0}, bino[8] = {0}, binn[8] = {0};
				ComputeLTP(GrayImage, i, j, s, binp, bino, binn);
				for ( p = 0; p < 8; p++){
					e[p+6].val[0] = s[p];
				}
				for ( p = 0; p < 14; p++){
					cvSet2D( points, k, p, e[p]);
				}
				k++;
			}
		}
		CvMat* pMean = cvCreateMat(1, 14, CV_32FC1);
		//pEigVals中的每个数表示一个特征值
		CvMat* pEigVals = cvCreateMat(1, 14, CV_32FC1);
		//每一行表示一个特征向量
		CvMat* pEigVecs = cvCreateMat( 14, 14, CV_32FC1);
		cvCalcPCA( points, pMean, pEigVals, pEigVecs, CV_PCA_DATA_AS_ROW );

		printf("Original Data:\n");
		PrintMatrix(points,SAMPLE_NUMBER,14);

		printf("==========\n");
		printf("\Mean Value:\n");
		PrintMatrix(pMean,1,14);

		printf("\nEigne Value:\n");
		PrintMatrix(pEigVals,1,14);

		printf("\nEigne Vector:\n");
		PrintMatrix(pEigVecs,14,14);
		
		CvMat* pResult = cvCreateMat( SAMPLE_NUMBER, 10, CV_32FC1 );
		cvProjectPCA( points, pMean, pEigVecs, pResult );
		CvMat* pRecon = cvCreateMat( SAMPLE_NUMBER, 14, CV_32FC1 );
		cvBackProjectPCA( pResult, pMean, pEigVecs, pRecon );

		printf("\nEigne Vector:\n");
		PrintMatrix(pResult,SAMPLE_NUMBER,10);
		
		printf("\nEigne Vector:\n");
		PrintMatrix(pRecon,SAMPLE_NUMBER,14);	
		
		
		cvShowImage("clusters", img );
		cvReleaseImage( &t);
		int c = cvWaitKey(0/*10*/);
		if(c=='p'){
			c = 0;
			while(c != 'p' && c !=27){
				c = cvWaitKey(250);
			}
		}
		if(c==27){
			return 0;
		}
	}
	cvWaitKey(0);
	cvReleaseImage(&img);
	cvDestroyAllWindows();
	cvReleaseMat(&points);
	return 0;
}