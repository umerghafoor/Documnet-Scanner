#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


using namespace cv;
using namespace std;


///////////////  Document Scanner //////////////////////

/////////////////////////////////////////////////////////////////////Declarations
Mat img, imggryblr, imgcany;
Mat imgcontr;
Mat imgwarp;
vector<Point> imgpoint;
vector<Point> newpoints;
float w = 420, h = 596;

/////////////////////////////////////////////////////////////////////Draw Contours
void drawconts(vector<Point> points, Scalar color)
{
	for (int i = 0; i < points.size(); i++)
	{
		circle(img, points[i], 4, color, FILLED);
//		putText(img, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 2);
	}
	line(img, points[0], points[1], color, 2);
	line(img, points[1], points[3], color, 2);
	line(img, points[2], points[3], color, 2);
	line(img, points[0], points[2], color, 2);
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
vector<Point> getContours(Mat imgcany,Mat img) {

	vector<vector<Point>> contours;
	vector<Vec4i> hirarchy;

	findContours(imgcany, contours, hirarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//	drawContours(img, contours, 0, Scalar(0, 0, 255), 4);

	vector<vector<Point>> contpoly(contours.size());
	vector<Rect> brect(contours.size());

	vector<Point> bigest;
	int Maxarea=0;

	for (int i = 0;i < contours.size();i++)
	{
		int area = contourArea(contours[i]);
		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], contpoly[i], 0.02 * peri, true);
		//	cout << endl << contpoly[i].size();
			if (contpoly[i].size() == 4 && area > Maxarea)
			{
				bigest = { contpoly[i][0],contpoly[i][1], contpoly[i][2], contpoly[i][3] };
				area = Maxarea;
			//	cout << endl << endl << "         working          " << endl << endl;
			//	drawContours(img, contours, i, Scalar(255, 0, 255), 2);
			}
			
		}
	}
	return bigest;
}
////////////////////////////////////////////////////////////////////Reorder

vector<Point> reorder(vector<Point> repoint)
{
	vector<Point> newpoint;
	vector<int> sump, subp;

	for (int i = 0;i < 4;i++)
	{
		sump.push_back(repoint[i].x + repoint[i].y);
		subp.push_back(repoint[i].x - repoint[i].y);
	}

	newpoint.push_back(repoint[min_element(sump.begin(), sump.end()) - sump.begin()]);
	newpoint.push_back(repoint[max_element(subp.begin(), subp.end()) - subp.begin()]);
	newpoint.push_back(repoint[min_element(subp.begin(), subp.end()) - subp.begin()]);
	newpoint.push_back(repoint[max_element(sump.begin(), sump.end()) - sump.begin()]);

	return newpoint;
}
////////////////////////////////////////////////////////////////////////////Warping

Mat warp(Mat img, vector<Point> point, float w, float h)
{
	Point2f scr[4] = { point[0],point[1], point[2], point[3] };
	Point2f dst[4] = { {0.0f,0.0f},{w,0.0f}, {0.0f,h}, {w,h} };

	Mat matrix = getPerspectiveTransform(scr, dst);
	warpPerspective(img, imgwarp, matrix, Point(w, h));

	float crpv = 5;
	Rect roi(crpv, crpv, w - (2 * crpv), h - (2 * crpv));
	imgwarp = imgwarp(roi);

	return imgwarp;
}


///////////////////////////////////////////MAIN FUNCTIONS////////////////////////////////////////////////////

//void main() {
//
//	string path = "Resources/paper.jpg";
//	img = imread(path);
//
////	imshow("Orignal Image", img);
//
//	imgcany = preprocess(img);
////	imshow("Pre process", imgcany);
//	
//	imgpoint = getContours(imgcany,img);
//	newpoints = reorder(imgpoint);
//
//	imgwarp = warp(img,newpoints,w,h);
//	imshow("Image warp", imgwarp);
//	imwrite("Resources/Scaned/1.png", imgwarp);
//
//	drawconts(newpoints, Scalar(0, 255, 255));
//	resize(img, img, Size(), 0.5, 0.5);
//	imshow("Orignal Image Points", img);
//
//	waitKey(0);
//}

////////////////////////////////////////////////////Main for vedio///////////////////////////////////////////

void main() {

	VideoCapture cap(0);
//	resize(img, img, Size(), 0.5, 0.5);

	while (true)
	{
		//	cout << "Start Working" << endl;
		cap.read(img);
		flip(img, img, 1);

		imgcany = preprocess(img);
	//	imshow("Pre process", imgcany);

		imgpoint = getContours(imgcany, img);
		if (!imgpoint.empty())
		{
			newpoints = reorder(imgpoint);

			imgwarp = warp(img, newpoints, w, h);
			flip(imgwarp, imgwarp, 1);
			imshow("Image warp", imgwarp);
			imwrite("Resources/Scaned/1.png", imgwarp);

			drawconts(newpoints, Scalar(0, 255, 255));
		}

		imshow("Orignal Image Points", img);
		//	cout << "End Working" << endl;
		waitKey(1);
	}
	
}