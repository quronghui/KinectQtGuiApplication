#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/types_c.h> 


using namespace std;
using namespace cv;

void addImage1(Mat image, Mat logoImage);

int main5645673(int argc, char *argv[])
{
	Mat logoImage = imread("bb.jpg");
	Mat image = imread("img.jpg");
	addImage1(image, logoImage);
	imshow("??", image);

	waitKey(0);
	return 0;
}

void addImage1(Mat image, Mat logoImage)
{
	cvtColor(logoImage, logoImage, CV_BGR2GRAY);
	Mat logo = Mat::zeros(logoImage.size(), CV_8UC1);
	vector<vector<Point>> contours;
	findContours(logoImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	drawContours(logo, contours, -1, Scalar(255), 2);



	imshow("!!", logo);
	waitKey(0);
	Mat imgToDo = image.clone();
	Mat imageROI = image(Range(0, logo.rows), Range(0, logo.cols));
	logo.copyTo(imageROI, logo);
	imshow("!!", logo);
	waitKey(0);

	//【3】将logo加到原图上
	Mat imageROI2 = imgToDo(Range(0, logo.rows), Range(0, logo.cols));
	addWeighted(imageROI, 0.5, imageROI2, 0.5, 0., imageROI);
	//imshow("!!", mask);
}