#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace std;

//Global Variables
Mat img, placeholder;


int main(int argc, const char** argv)
{
	// filename
	// Read the input image
	int image_number = 2;
	int nImages = 10;

	char filename[20];
	sprintf_s(filename, "images/rub%02d.jpg", image_number%nImages);
	img = imread(filename);
	// Resize the image to 400x400
	Size rsize(400, 400);
	resize(img, img, rsize);

	/*if (img.empty())
	{
		return -1;
	}*/

	// Create an empty window
	namedWindow("PRESS P for Previous, N for Next Image", WINDOW_AUTOSIZE);
	// Create a callback function for any event on the mouse

	imshow("PRESS P for Previous, N for Next Image", img);
	while (1)
	{
		imshow("PRESS P for Previous, N for Next Image", img);
		char k = waitKey(1) & 0xFF;
		if (k == 27)
			break;
		//Check next image in the folder
		if (k == 'n')
		{
			image_number++;
			sprintf_s(filename, "images/rub%02d.jpg", image_number%nImages);
			img = imread(filename);
			resize(img, img, rsize);
			printf("here");
		}
		//Check previous image in he folder
		else if (k == 'p')
		{
			image_number--;
			sprintf_s(filename, "images/rub%02d.jpg", image_number%nImages);
			img = imread(filename);
			resize(img, img, rsize);
		}
	}
	return 0;
}