// 图像处理.cpp : 定义控制台应用程序的入口点。
//
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <cmath>
#include "cxcore.h"
using namespace cv;
using namespace std;
int OutputWidth  = 300;
int OutputHeight = 300; 									
#define MAX_CLUSTERS 2										//类别数
#define SAMPLE_NUMBER 100									//样本数 
#define PI 3.14159265
int hold = 2;												//LTP阈值
int kernel = 3;												//LTP直方图 n*n算子

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
			s[p] = 0.6;
		else if ( (bino[p] > binp[p])&&(bino[p] > binn[p]))
			s[p] = 0.3;
		else 
			s[p] = 0;
	}
}
int main(int argc, char* argv[])
{
	CvSize size;
	size.width  = OutputWidth;
	size.height = OutputHeight;
	IplImage* img = cvCreateImageHeader( size, IPL_DEPTH_8U, 3);
	IplImage* hsv = cvCreateImage( size, IPL_DEPTH_8U, 3);
	IplImage* GrayImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	//CvMat * mat = cvCreateMat(img->height, img->width, CV_64FC3);

	CvScalar color_tab[5];
	CvRNG rng = CvRNG( cvGetTickCount() );

	color_tab[0] = CV_RGB( 255, 0  ,   0);
	color_tab[1] = CV_RGB( 0  , 255,   0);
	color_tab[2] = CV_RGB( 255, 0  , 255);
	color_tab[3] = CV_RGB( 255, 255,   0);
	color_tab[4] = CV_RGB( 0  , 255, 255);

	cvNamedWindow("clusters",CV_WINDOW_AUTOSIZE);

	int k, p, cluster_count = MAX_CLUSTERS;
	int i, j,  sample_count = SAMPLE_NUMBER;
	float s[8] = {0};	


	CvMat* points   = cvCreateMat( sample_count, 14, CV_32FC1);					//数据样本
	CvMat* points2  = cvCreateMat( sample_count, 1, CV_32FC2);					//储存坐标信息
	CvMat* clusters = cvCreateMat( sample_count, 1, CV_32SC1);					//标签
	CvMat* centers  = cvCreateMat(cluster_count, 14, CV_32FC2);					

	((CvPoint2D32f*)centers->data.fl)[0].x = 192/3;								//初始化中心点
	((CvPoint2D32f*)centers->data.fl)[0].y = 288/3;
	((CvPoint2D32f*)centers->data.fl)[1].x = 384/3;
	((CvPoint2D32f*)centers->data.fl)[1].y = 432/3;
	((CvPoint2D32f*)centers->data.fl)[2].x = 576/3;
	((CvPoint2D32f*)centers->data.fl)[2].y = 288/3;
	((CvPoint2D32f*)centers->data.fl)[3].x = 384/3;
	((CvPoint2D32f*)centers->data.fl)[3].y = 144/3;

	while(1){
		//double time = (double)getTickCount(); 
		/*CaptureFromCG(img);*/
		img = cvLoadImage("4.jpg");
		IplImage *t = cvCloneImage(img);
		cvCvtColor(img, hsv      , CV_BGR2HSV );
		cvCvtColor(img, GrayImage, CV_BGR2GRAY);
		/*多元高斯分布生成随机样本*/
		k=0;
		while (k < sample_count){		
														//特征提取
			i = cvRandInt(&rng)%img->width;
			j = cvRandInt(&rng)%img->height;
			if (( i > 3*(kernel-1)/2) && ( j > 3*(kernel-1)/2) && ( i < OutputWidth-3*(kernel-1)/2-1) && ( j < OutputHeight-1-3*(kernel-1)/2)){
				((CvPoint2D32f*)points2->data.fl)[k].x = i;
				((CvPoint2D32f*)points2->data.fl)[k].y = j;

				CvScalar e[14];
				e[0].val[0]=(float)cvGet2D(hsv, j, i).val[0]/180;			//HSV三通道
				e[1].val[0]=(float)cvGet2D(hsv, j, i).val[1]/256;
				e[2].val[0]=(float)cvGet2D(hsv, j, i).val[2]/256;
				e[3].val[0]=(float)cvGet2D(img, j, i).val[0]/256;			//RGB三通道
				e[4].val[0]=(float)cvGet2D(img, j, i).val[1]/256;
				e[5].val[0]=(float)cvGet2D(img, j, i).val[2]/256;

				//e[0].val[0]=atan((float)cvGet2D(hsv, j, i).val[0])*2/PI;			//HSV三通道		//归一化
				//e[1].val[0]=atan((float)cvGet2D(hsv, j, i).val[1])*2/PI;
				//e[2].val[0]=atan((float)cvGet2D(hsv, j, i).val[2])*2/PI;
				//e[3].val[0]=atan((float)cvGet2D(img, j, i).val[0])*2/PI;			//RGB三通道
				//e[4].val[0]=atan((float)cvGet2D(img, j, i).val[1])*2/PI;
				//e[5].val[0]=atan((float)cvGet2D(img, j, i).val[2])*2/PI;
				int binp[8] = {0}, bino[8] = {0}, binn[8] = {0};
				ComputeLTP(GrayImage, i, j, s, binp, bino,binn);
				for ( p = 0; p < 8; p++){
					e[p+6].val[0] = s[p];
				}
				for ( p = 0; p < 14; p++){
					cvSet2D( points, k, p, e[p].val[0]);
				}
				k++;
			}
		}
		cvKMeans2( points, cluster_count, clusters, cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0), 10, 0, 0, centers, 0);
//		kmeans(points, cluster_count, clusters,
//			TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 1.0),
//			3, KMEANS_PP_CENTERS, centers);
//		kmeans(points, cluster_count, clusters, cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0), 1, KMEANS_RANDOM_CENTER, centers);
/*		float posi = 0, nega = 0;
		int nposi = 0, nnega = 0;		
		for ( k = 0; k < sample_count; k ++){
			CvPoint2D32f pt = ((CvPoint2D32f*)points2->data.fl)[k];	
			int cluster_idx = clusters->data.i[k];
			if (cluster_idx == 0){
				posi = posi + pt.y;
				nposi++;
			} 
			else{
				nega = nega + pt.y;
				nnega++;
			}
			cvCircle( img, cvPointFrom32f(pt), 1, color_tab[cluster_idx ], CV_FILLED);
		}
		//for( i = 0; i < cluster_count; i++){												//质心点
		//	CvPoint2D32f pt = ((CvPoint2D32f*)centers->data.fl)[i];			
		//	cvCircle( img, cvPointFrom32f(pt), 4, color_tab[	i		], CV_FILLED);
		/*}*/
		cvShowImage("clusters", img );
		cvReleaseImage( &t);
		
/*		FILE* file1=fopen("points.txt","w+");
		for(i=0;i<points->rows;i++){  
			for(j=0;j<points->cols;j++){  				
				fprintf(file1,"%f ",cvGet2D( points, i, j ));  
			}  
			fprintf(file1,"\n");  
		}  
		fclose(file1);
		FILE* file2=fopen("clusters.txt","w+");
		if( (posi/nposi)>(nega/nnega) ){
			for (k = 0; k<sample_count; k++){
				fprintf(file2,"%2d",clusters->data.i[k]);
				/*fprintf(file,"\n");*/
/*			}
		}
		else{
			for (k = 0; k<sample_count; k++){
				fprintf(file2,"%2d",1-clusters->data.i[k]);
				/*fprintf(file,"\n");*/
/*			}
		}
		fclose(file2);
		FILE* file3=fopen("locations.txt","w+");
		for(i=0;i<points2->rows;i++){  
			for(j=0;j<points2->cols;j++){  				
				fprintf(file1,"%f ",cvGet2D( points2, i, j ));  
			}  
			fprintf(file1,"\n");  
		}  
		fclose(file3);
*/
		int c = cvWaitKey(0);
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
	cvReleaseMat(&clusters);
	return 0;
}