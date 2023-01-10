#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

using namespace cv;
using namespace std;


///////////////  Document Scanner //////////////////////

/////////////////////////////////////////////////////////////////////Declarations
Mat img, imggryblr, imgcany;
Mat imgcontr;
vector<Point> imgpoint;

/////////////////////////////////////////////////////////////////Draw Contours
void drawcont(vector<Point> point, Scalar color)
{
	for (int i = 0;i < point.size();i++)
	{
		circle(img, point[i], 30, color, FILLED);
		//	putText(img, (string)i, )
	}
}

/////////////////////////////////////////////////////////////////////PreProcecing
Mat preprocess(Mat img)
{
	cvtColor(img, imggryblr, COLOR_BGR2GRAY);
	GaussianBlur(imggryblr, imggryblr, Size(5, 5), 5, 0);
	Canny(imggryblr, imgcany, 30, 100);
	Mat k = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgcany, imgcany, k);
	return imgcany;
}
/////////////////////////////////////////////////////////////////////Get Contours
vector<Point> getContours(Mat imgcany) {

	vector<vector<Point>> contours;
	vector<Vec4i> hirarchy;

	findContours(imgcany, contours, hirarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> contpoly(contours.size());
	vector<Rect> brect(contours.size());
	vector<Point> bigest;
	int Maxarea=0;

	for (int i = 0;i < contours.size();i++)
	{
		int area = contourArea(contours[i]);
		if (area > 100)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], contpoly[i], 0.02 * peri, true);
			if (contpoly[i].size() == 4 && area < Maxarea)
			{
				drawContours(img, contpoly, i, Scalar(255, 0, 255), 5);
				bigest = { contpoly[i][0],contpoly[i][1], contpoly[i][2], contpoly[i][3] };
				area = Maxarea;
				cout << endl << endl << "working" << endl << endl;
				
			}
			
		}
	}
	return bigest;
}

///////////////////////////////////////////MAIN FUNCTIONS////////////////////////////////////////////////////

void main() {

	string path = "Resources/paper.jpg";
	Mat img = imread(path);
	resize(img, img, Size(), 0.5, 0.5);

	imshow("Orignal Image", img);

	imgcany = preprocess(img);
	imshow("Pre process", imgcany);


	imgpoint = getContours(imgcany);
	drawcont(imgpoint, Scalar(255, 0, 155));


	waitKey(0);
}