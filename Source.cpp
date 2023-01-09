#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

using namespace cv;
using namespace std;

///////////////  WebCam //////////////////////

int main() {

	VideoCapture cap(0);
	Mat img;
	while (1)
	{
		cap.read(img);

		imshow("Image", img);
		waitKey(10);
	}

}

/////////////////  Importing Video //////////////////////
//
//int main() {
//
//	string path = "Resources/test_video.mp4";
//	VideoCapture cap(path);
//	Mat img;
//	while (1)
//	{
//		cap.read(img);
//
//		imshow("Image", img);
//		waitKey(10);
//	}
//	
//}

/////////////////  Importing Images //////////////////////
//
//int main() {
//
//	string path = "Resources/test.png";
//	Mat img = imread(path);
//	imshow("Image", img);
//	waitKey(0);
//}