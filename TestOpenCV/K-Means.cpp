// 图像处理.cpp : 定义控制台应用程序的入口点。
//
#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <cmath>
#include "cxcore.h"

int OutputWidth  = 960;
int OutputHeight = 540; 									
#define MAX_CLUSTERS 2										//类别数
#define SAMPLE_NUMBER 100000								//样本数 
int kernel = 3;												//LTP直方图 n*n算子

int main(int argc, char* argv[])
{
	CvSize size;
	size.width  = OutputWidth;
	size.height = OutputHeight;
	IplImage* img = cvCreateImageHeader( size, IPL_DEPTH_8U, 3);

	CvScalar color_tab[5];
	CvRNG rng = CvRNG(0xffffffff);

	color_tab[0] = CV_RGB( 255, 0  ,   0);
	color_tab[1] = CV_RGB( 0  , 255,   0);
	color_tab[2] = CV_RGB( 255, 0  , 255);
	color_tab[3] = CV_RGB( 255, 255,   0);
	color_tab[4] = CV_RGB( 0  , 255, 255);

	cvNamedWindow("clusters",CV_WINDOW_AUTOSIZE);

	int k, p, cluster_count = MAX_CLUSTERS;
	int i, j,  sample_count = SAMPLE_NUMBER;

	CvMat* points   = cvCreateMat( sample_count, 3, CV_32FC2);					//数据样本
	CvMat* points2  = cvCreateMat( sample_count, 1, CV_32FC2);					//储存坐标信息
	CvMat* clusters = cvCreateMat( sample_count, 1, CV_32SC1);					//标签
	CvMat* centers  = cvCreateMat(cluster_count, 3, CV_32FC2);					

	while(1){

		img = cvLoadImage("Test.png");
		IplImage *t = cvCloneImage(img);
		/*多元高斯分布生成随机样本*/
		k=0;
		while (k < sample_count){		
														//特征提取
			i = cvRandInt(&rng)%img->width;
			j = cvRandInt(&rng)%img->height;
			((CvPoint2D32f*)points2->data.fl)[k].x = i;
			((CvPoint2D32f*)points2->data.fl)[k].y = j;

			CvScalar e[3];
			e[0].val[0]=(float)cvGet2D(img, j, i).val[0];			//RGB三通道
			e[1].val[0]=(float)cvGet2D(img, j, i).val[1];
			e[2].val[0]=(float)cvGet2D(img, j, i).val[2];


			for ( p = 0; p < 3; p++){
					cvSet2D( points, k, p, e[p].val[0]);
				}
				k++;
			}
		
		for (i = 0; i < sample_count / 2; i++)
		{
			CvPoint2D32f* pt1 = (CvPoint2D32f*)points->data.fl +
				cvRandInt(&rng) % sample_count;
			CvPoint2D32f* pt2 = (CvPoint2D32f*)points->data.fl +
				cvRandInt(&rng) % sample_count;
			CvPoint2D32f temp;
			CV_SWAP(*pt1, *pt2, temp);
		}
		
		cvKMeans2(points, cluster_count, clusters, 
			cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 
				10, 1.0),10, 0, 0, centers, 0);

		float posi = 0, nega = 0;
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
		printf("%d,%d\n", ((CvPoint2D32f*)centers->data.fl)[0].x, ((CvPoint2D32f*)centers->data.fl)[0].y);
		printf("%d,%d\n", ((CvPoint2D32f*)centers->data.fl)[1].x, ((CvPoint2D32f*)centers->data.fl)[1].y);
		for( i = 0; i < cluster_count; i++){												//质心点
			CvPoint2D32f pt3 = ((CvPoint2D32f*)centers->data.fl)[i];			
			cvCircle( img, cvPointFrom32f(pt3), 20, color_tab[i], CV_FILLED);
		}
		cvShowImage("clusters", img );
		cvReleaseImage( &t);
		
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