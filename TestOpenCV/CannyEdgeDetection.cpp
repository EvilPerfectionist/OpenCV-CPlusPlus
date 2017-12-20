/**************
 *	ÉùÃ÷Ä£¿é  *
 **************/
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

int lowThreshold      =  80;  
int max_lowThreshold  = 100;  

int main(){  
	CvSize size;
	size.width  = 768;
	size.height = 576;
	IplImage* img = cvCreateImageHeader(size,IPL_DEPTH_8U,3);
	IplImage* src_gray = cvCreateImage(size,IPL_DEPTH_8U,1);  
	IplImage* detected_edges = cvCreateImage(size,IPL_DEPTH_8U,1); 
	
	int edgeThresh = 1;  
	
	int ratio = 3;  
	int kernel_size = 3;  
	char* window_name = "Edge Map";  
	
	cvNamedWindow( "src", CV_WINDOW_AUTOSIZE ); 
	cvNamedWindow( window_name, CV_WINDOW_AUTOSIZE );  
	while (1){
		img = cvLoadImage("38.jpg");
		IplImage *t = cvCloneImage(img);
		
		cvCvtColor( img, src_gray, CV_BGR2GRAY);
		cvCanny( src_gray, detected_edges, lowThreshold , max_lowThreshold , kernel_size );
		cvFlip(detected_edges);
		cvShowImage( "src", img );
		cvShowImage( window_name, detected_edges );
		
		cvReleaseImage( &t);
		int c = cvWaitKey(33);
		if(c=='p'){
			c = 0;
			while(c != 'p' && c !=27){
				c = cvWaitKey(250);
			}
		}
		if(c==27)
			return 0;
	}
	cvWaitKey(0);
	cvReleaseImage(&img);
	cvReleaseImage(&src_gray);
	cvReleaseImage(&detected_edges);
	cvDestroyAllWindows();
	return 0;  
}