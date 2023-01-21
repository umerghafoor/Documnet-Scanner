#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

//using namespace cv;
//using namespace std;

////////////////////////////////////////////////////////
///////////////  Document Scanner //////////////////////
////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////Declarations
cv::Mat img, imggryblr, imgcany;
cv::Mat imgcontr;
cv::Mat imgwarp;
cv::Mat imgclr;
std::vector<cv::Point> imgpoint;
std::vector<cv::Point> newpoints;
float w = 420, h = 596;

/////////////////////////////////////////////////////////////////////Draw Contours
void drawconts(std::vector<cv::Point> points, cv::Scalar color)
{
	for (int i = 0; i < points.size(); i++)
	{
		circle(img, points[i], 4, color, cv::FILLED);
//		putText(img, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 2);
	}
	line(img, points[0], points[1], color, 2);
	line(img, points[1], points[3], color, 2);
	line(img, points[2], points[3], color, 2);
	line(img, points[0], points[2], color, 2);
}

/////////////////////////////////////////////////////////////////////PreProcecing
cv::Mat preprocess(cv::Mat img)
{
	cvtColor(img, imggryblr, cv::COLOR_BGR2GRAY);
	GaussianBlur(imggryblr, imggryblr, cv::Size(5, 5), 5, 0);
	Canny(imggryblr, imgcany, 30, 100);
	cv::Mat k = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	dilate(imgcany, imgcany, k);
	return imgcany;
}
/////////////////////////////////////////////////////////////////////Get Contours
std::vector<cv::Point> getContours(cv::Mat imgcany, cv::Mat img) {

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hirarchy;

	findContours(imgcany, contours, hirarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
//	drawContours(img, contours, 0, Scalar(0, 0, 255), 4);

	std::vector<std::vector<cv::Point>> contpoly(contours.size());
	std::vector<cv::Rect> brect(contours.size());

	std::vector<cv::Point> bigest;
	int Maxarea=0;

	for (int i = 0;i < contours.size();i++)
	{
		int area = contourArea(contours[i]);
		if (area > 10000)
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
		//drawContours(img, contours, i, cv::Scalar(255, 0, 255), 2);
	}
	return bigest;
}
////////////////////////////////////////////////////////////////////Reorder

std::vector<cv::Point> reorder(std::vector<cv::Point> repoint)
{
	std::vector<cv::Point> newpoint;
	std::vector<int> sump, subp;

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

cv::Mat warp(cv::Mat img, std::vector<cv::Point> point, float w, float h)
{
	cv::Point2f scr[4] = { point[0],point[1], point[2], point[3] };
	cv::Point2f dst[4] = { {0.0f,0.0f},{w,0.0f}, {0.0f,h}, {w,h} };

	cv::Mat matrix = getPerspectiveTransform(scr, dst);
	warpPerspective(img, imgwarp, matrix, cv::Point(w, h));

	float crpv = 5;
	cv::Rect roi(crpv, crpv, w - (2 * crpv), h - (2 * crpv));
	imgwarp = imgwarp(roi);

	return imgwarp;
}


///////////////////////////////////////////MAIN FUNCTIONS////////////////////////////////////////////////////
//
//void main() {
//
//	string path = "Resources/img.jpg";
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

	int camerano = 0;
	std::cout << "Enter the camera ID" << std::endl;
	std::cout << "0-For Default camera " << std::endl;
	std::cout << "1-If Additional webcam is attached " << std::endl;
	std::cin >> camerano;
	cv::VideoCapture cap(camerano);
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
			//Canny(imgwarp, imgclr, 30, 10);
			//imshow("Image clear", imgclr);
			drawconts(newpoints, cv::Scalar(0, 255, 255));
		}

		imshow("Orignal Image Points", img);
		//	cout << "End Working" << endl;
		cv::waitKey(1);
	}
	
}