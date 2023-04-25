#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

////////////////////////////////////////////////////////
///////////////  Document Scanner //////////////////////
////////////////////////////////////////////////////////

/* These are variable declarations in C++ for an image processing program. */
cv::Mat img, imggryblr, imgcany;
cv::Mat imgcontr;
cv::Mat imgwarp;
cv::Mat imgclr;
std::vector<cv::Point> imgpoint;
std::vector<cv::Point> newpoints;
float w = 420, h = 596;

/**
 * The function draws a set of points and lines to form a contour on an image.
 * 
 * @param points A vector of cv::Point objects representing the four corners of a rectangle.
 * @param color The color of the lines and circles to be drawn, specified as a cv::Scalar object.
 */
void drawconts(std::vector<cv::Point> points, cv::Scalar color)
{
	for (int i = 0; i < points.size(); i++)
	{
		circle(img, points[i], 4, color, cv::FILLED);
	}
	line(img, points[0], points[1], color, 2);
	line(img, points[1], points[3], color, 2);
	line(img, points[2], points[3], color, 2);
	line(img, points[0], points[2], color, 2);
}

/**
 * The function preprocesses an input image by converting it to grayscale, applying Gaussian blur,
 * Canny edge detection, and dilation.
 * 
 * @param img The input image that needs to be preprocessed.
 * 
 * @return The function `preprocess` returns a `cv::Mat` object, which is the processed image after
 * applying various image processing techniques such as converting to grayscale, Gaussian blur, Canny
 * edge detection, and dilation.
 */
cv::Mat preprocess(cv::Mat img)
{
	cvtColor(img, imggryblr, cv::COLOR_BGR2GRAY);
	GaussianBlur(imggryblr, imggryblr, cv::Size(5, 5), 5, 0);
	Canny(imggryblr, imgcany, 30, 100);
	cv::Mat k = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	dilate(imgcany, imgcany, k);
	return imgcany;
}

/**
 * The function takes in an image and its canny edge detection output, finds the contours of the image,
 * approximates the contours to polygons, and returns the largest quadrilateral polygon found.
 * 
 * @param imgcany A binary image obtained after applying Canny edge detection on the original image.
 * @param img The input image on which the contours are drawn.
 * 
 * @return A vector of cv::Point representing the four corners of the largest quadrilateral contour in
 * the input image that has an area greater than 10000.
 */
std::vector<cv::Point> getContours(cv::Mat imgcany, cv::Mat img) {

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hirarchy;

	findContours(imgcany, contours, hirarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

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
			if (contpoly[i].size() == 4 && area > Maxarea)
			{
				bigest = { contpoly[i][0],contpoly[i][1], contpoly[i][2], contpoly[i][3] };
				area = Maxarea;
			}
			
		}
	}
	return bigest;
}
/**
 * The function reorders a vector of points based on their x and y coordinates.
 * 
 * @param repoint A vector of cv::Point objects representing the four corners of a quadrilateral.
 * 
 * @return The function `reorder` returns a `std::vector<cv::Point>` which contains the reordered
 * points.
 */
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
/**
 * The function warps an input image based on four given points and returns the warped image with a
 * cropped region.
 * 
 * @param img The input image that needs to be warped.
 * @param point A vector of 4 points representing the corners of a quadrilateral in the input image
 * that needs to be warped.
 * @param w width of the output image after perspective transformation
 * @param h The height of the output image after perspective transformation.
 * 
 * @return a cv::Mat object, which is the warped image after applying perspective transformation and
 * cropping.
 */
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

void main() {

	/* This code is asking the user to input the camera ID to be used for capturing the video. If the user
	enters 0, the default camera will be used, and if the user enters 1, an additional webcam attached
	to the system will be used. The input camera ID is stored in the variable `camerano`, which is then
	used to initialize the `cv::VideoCapture` object `cap`. */
	int camerano = 0;
	std::cout << "Enter the camera ID" << std::endl;
	std::cout << "0-For Default camera " << std::endl;
	std::cout << "1-If Additional webcam is attached " << std::endl;
	std::cin >> camerano;
	cv::VideoCapture cap(camerano);

	/* The code is continuously capturing frames from a camera and processing them to detect a rectangular
	document in the frame. Once a document is detected, the code applies perspective transformation to
	warp the document and crop it to a specific size. The warped image is then displayed and saved as a
	PNG file. The `while (true)` loop ensures that the code keeps running until the user manually stops
	it. */
	while (true)
	{
		cap.read(img);
		flip(img, img, 1);

		imgcany = preprocess(img);

		imgpoint = getContours(imgcany, img);
		if (!imgpoint.empty())
		{
			newpoints = reorder(imgpoint);

			imgwarp = warp(img, newpoints, w, h);
			flip(imgwarp, imgwarp, 1);
			imshow("Image warp", imgwarp);
			imwrite("Resources/Scaned/1.png", imgwarp);
			drawconts(newpoints, cv::Scalar(0, 255, 255));
		}

		imshow("Orignal Image Points", img);
		cv::waitKey(1);
	}
	
}