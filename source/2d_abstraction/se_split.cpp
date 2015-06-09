
#include <string>
#include <iostream>
#include <ceres/ceres.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "util.h"
#include "image_output.h"

using namespace cv;
using namespace std;
using ceres::Solver;




bool sortPointsByX(cv::Point2d &p1, cv::Point2d &p2) {
	return p1.x < p2.x;
}

bool sortPointsByY(cv::Point2d &p1, cv::Point2d &p2) {
	return p1.y < p2.y;
}



bool isLeftOf(tuple<double, double> testPoint, tuple<double, double> point1, tuple<double, double> point2) {

	double det = (get<0>(point2) -get<0>(point1))*(get<1>(testPoint) - get<1>(point1)) - (get<0>(testPoint) -get<0>(point1))*(get<1>(point2) -get<1>(point1));
	

	if (det > 0) {
		return true;
	}
	else {
		return false;
	}
}


bool isLeftOf(cv::Point testPoint, cv::Point point1, cv::Point point2) {

	double det = (point2.x - point1.x)*(testPoint.y - point1.y) - (testPoint.x - point1.x)*(point2.y - point1.y);


	if (det > 0) {
		return true;
	}
	else {
		return false;
	}
}














void splitImageToGrids(int* pixelGrid, const int width, const int height, double xc, double yc, double theta, vector<int> &splitPart1, vector<int> &splitPart2) {
		
	const int numPixels = width * height;

	tuple<double, double> point1;
	tuple<double, double> point2;

	//cout << "theta: " << theta << endl;
	//cout << "xc: " << xc << endl;
	//cout << "yc: " << yc << endl;

	//rotate 90 deg
	theta += 1.57079;


	if (theta == 0) {
		point1 = make_tuple(0, yc);
		point2 = make_tuple(width, yc);
	}
	else {
		double tanTheta = tan(theta);
		double t = yc - tanTheta*xc;

		//cout << "tanTheta: " << tanTheta << endl;
		//cout << "t: " << t << endl;

		
		int y1 = int(t);
		point1 = make_tuple(0, y1);
		//cout << "point1: " << 0 << ", " << y1 << endl;

		int y2 = int(tanTheta * width + t);
		point2 = make_tuple(width, y2);
		//cout << "point2: " << width << ", " << y2 << endl;

	}


	double x, y;
	for (int i = 0; i < width*height; i++) {
		if (pixelGrid[i] != 0) {
			y = i / width;
			x = i % width;
			tuple<double, double> testPoint(x, y);
			if (isLeftOf(testPoint, point1, point2)){
				splitPart1.at(i) = 100;
			}
			else {
				splitPart2.at(i) = 100;
			}
		}
	}

}



/**
* This method splits a pixelGrid given as input into two binary Mats (1 channel, uchar) along a line given by a point xc,yc and an angle theta.
*
*
*/


void splitImageToMats(vector<int> pixelGrid, const int width, const int height, double xc, double yc, double theta, Mat &splitPart1, Mat &splitPart2) {
	Mat internPart1 = Mat(height, width, CV_8UC1);
	Mat internPart2 = Mat(height, width, CV_8UC1);
	internPart1.setTo((uchar)0);
	internPart2.setTo((uchar)0);

	const int numPixels = width * height;

	tuple<double, double> point1;
	tuple<double, double> point2;

	//cout << "theta: " << theta << endl;
	//cout << "xc: " << xc << endl;
	//cout << "yc: " << yc << endl;

	//rotate 90 deg
	theta += 1.57079;


	if (theta == 0) {
		point1 = make_tuple(0, yc);
		point2 = make_tuple(width, yc);
	}
	else {
		double tanTheta = tan(theta);
		double t = yc - tanTheta*xc;

		//cout << "tanTheta: " << tanTheta << endl;
		//cout << "t: " << t << endl;


		int y1 = int(t);
		point1 = make_tuple(0, y1);
		//cout << "point1: " << 0 << ", " << y1 << endl;

		int y2 = int(tanTheta * width + t);
		point2 = make_tuple(width, y2);
		//cout << "point2: " << width << ", " << y2 << endl;

	}


	double x, y;
	for (int i = 0; i < width*height; i++) {
		if (pixelGrid[i] != 0) {
			y = i / width;
			x = i % width;
			Point point = Point(x, y);
			tuple<double, double> testPoint(x, y);
			if (isLeftOf(testPoint, point1, point2)){
				internPart1.at<uchar>(point) = (uchar)1;
			}
			else {
				internPart2.at<uchar>(point) = (uchar)1;
			}
		}
	}
	splitPart1 = internPart1;
	splitPart2 = internPart2;
}






/**
* This method splits a contour along a line given by a point xc,yc and an angle theta.
*
*
*/


void splitContourToVectors(vector<cv::Point> contourPoints, const int width, const int height, const double xc, const double yc, const double theta, vector<cv::Point> &splitPart1, vector<cv::Point> &splitPart2) {

	//rotate 90 deg to split orthogonal to the ellipse orientation
	double newTheta = theta + 1.57079;


	//=================================================
	//==    calculate endpoints of the split line    ==
	//=================================================
	cv::Point point1;
	cv::Point point2;

	//split line is horizontal
	if (newTheta == 0) {
		point1 = cv::Point(0, yc);
		point2 = cv::Point(width, yc);
	}
	//split line is vertical
	else if (theta < 0.001) {
		point1 = cv::Point(xc, 0);
		point2 = cv::Point(xc, height);
	}
	else {
		//use y = m*x +t:
		double m = tan(newTheta);	//m is tan(theta)
		double t = yc - m*xc;		//calculate t by using the known point xc,yc

		//calculate point1 on the left side of the image
		int y1 = int(t);

		//point1 is above the image -> calculate point1 on the top of the image
		if (y1 > height) {
			double x1d = (height - t) / m;
			int x1 = (int)x1d;
			point1 = cv::Point(x1, height);
		}
		//point1 is below the image -> calculate point1 on the bottom of the image
		else if (y1 < 0) {
			double x1d = (0 - t) / m;
			int x1 = (int)x1d;
			point1 = cv::Point(x1, 0);
		}
		//point1 is fine
		else {
			point1 = cv::Point(0, y1);
		}

		
		//calculate point2 on the right side of the image
		int y2 = int(m * width + t);

		//point2 is above the image -> calculate point2 on the top of the image
		if (y2 > height) {
			double x2d = (height - t) / m;
			int x2 = (int)x2d;
			point2 = cv::Point(x2, height);
		}
		//point2 is below the image -> calculate point2 on the bottom of the image
		else if (y2 < 0) {
			double x2d = (0 - t) / m;
			int x2 = (int)x2d;
			point2 = cv::Point(x2, 0);
		}
		//point2 is fine
		else {
			point2 = cv::Point(width, y2);
		}

	}

	//=================================================
	//==    split the contour using the split line   ==
	//=================================================

	//test each contour point against the split line and add it to the corresponding split part
	for (int i = 0; i < contourPoints.size(); i++) {
		cv::Point testPoint = contourPoints.at(i);

		if (isLeftOf(testPoint, point1, point2)){
			splitPart1.push_back(testPoint);
		}
		else {
			splitPart2.push_back(testPoint);
		}
	}

	//image_output::renderContourColored("testContour.png", width, height, contourPoints);
	//image_output::renderLineToImage("testSplitLine.png", width, height, point1, point2);



	//=======================================================================
	//==    determine the parts of the split line inside of the contour    ==
	//=======================================================================


	vector<cv::Point2d> intersectionPoints;

	//test each part of the contour for intersection with the split line
	cv::Point lastContourPoint = contourPoints.at(contourPoints.size() - 1);
	for (int i = 0; i < contourPoints.size(); i++) {
		cv::Point currentContourPoint = contourPoints.at(i);
		//do they intersect?
		int collide = util::testSegmentsIntersection(lastContourPoint, currentContourPoint, point1, point2);

		//segments do intersect
		if (collide == 13) {
			cv::Point2d intersect = util::intersectLines(lastContourPoint, currentContourPoint, point1, point2);

			//prevent double insertion of equal coordinates
			bool contained = false;
			for (int k = 0; k < intersectionPoints.size(); k++) {
				cv::Point2d testIntersect = intersectionPoints.at(k);
				if (intersect.x == testIntersect.x && intersect.y == testIntersect.y) {
					contained = true;
				}
			}
			if (!contained) {
				intersectionPoints.push_back(intersect);
			}
		}
		lastContourPoint = currentContourPoint;
	}


	//number of intersection points is not even: Fuckup with this method of comupation
	if (intersectionPoints.size() % 2 == 1) {
		cerr << "FUCKUP" << endl;

		//USE OTHER ALGO:

		//rasterize the split line
		vector<cv::Point> splitLine = util::bresenhamLine(point1, point2);

		//test for each point of the splitLine if it is contained in the original contour
		for (int i = 0; i < splitLine.size(); i++) {
			cv::Point testPoint = splitLine.at(i);
			double val = cv::pointPolygonTest(contourPoints, testPoint, false);
			//point is inside or on the edge of the contour
			if (val >= 0) {
				//add the contained points of the split line to both splitParts
				splitPart1.push_back(testPoint);
				splitPart2.push_back(testPoint);
			}
		}
	}


	//number of intersection points is even
	else {
		//sort the intersectionPoints along the split line 

		//split line is vertical: sort by y-coordinate
		if (theta < 0.001) {
			sort(intersectionPoints.begin(), intersectionPoints.end(), sortPointsByY);
		}

		//sort by x coordinate
		else {
			sort(intersectionPoints.begin(), intersectionPoints.end(), sortPointsByX);
		}

		//add the pieces between the intersection points to both split parts
		for (int i = 0; i < intersectionPoints.size() / 2; i++) {
			cv::Point2d intersection1 = intersectionPoints.at(2*i);
			cv::Point2d intersection2 = intersectionPoints.at(2*i + 1);

			cv::Point inter1 = cv::Point(intersection1.x, intersection1.y);
			cv::Point inter2 = cv::Point(intersection2.x, intersection2.y);

			//rasterize the part of the split line
			vector<cv::Point> splitLinePart = util::bresenhamLine(inter1, inter2);
			//add the points to both split parts
			for (int j = 0; j < splitLinePart.size(); j++) {
				cv::Point testPoint = splitLinePart.at(j);
				splitPart1.push_back(testPoint);
				splitPart2.push_back(testPoint);
			}
		}
	}

}











void splitContourToVectorsOld(vector<cv::Point> contourPoints, const int width, const int height, const double xc, const double yc, const double theta, vector<cv::Point> &splitPart1, vector<cv::Point> &splitPart2) {

	const int numPixels = width * height;

	cv::Point point1;
	cv::Point point2;


	//rotate 90 deg to split orthogonal to the ellipse orientation
	double newTheta = theta + 1.57079;


	//split line is horizontal
	if (newTheta == 0) {
		point1 = cv::Point(0, yc);
		point2 = cv::Point(width, yc);
	}
	//split line is vertical
	else if (theta < 0.001) {
		point1 = cv::Point(xc, 0);
		point2 = cv::Point(xc, height);
	}
	else {
		//use y = m*x +t:
		double m = tan(newTheta);	//m is tan(theta)
		double t = yc - m*xc;		//calculate t by using the known point xc,yc

		//calculate point1 on the left side of the image
		int y1 = int(t);

		//point1 is above the image -> calculate point1 on the top of the image
		if (y1 > height) {
			double x1d = (height - t) / m;
			int x1 = (int)x1d;
			point1 = cv::Point(x1, height);
		}
		//point1 is below the image -> calculate point1 on the bottom of the image
		else if (y1 < 0) {
			double x1d = (0 - t) / m;
			int x1 = (int)x1d;
			point1 = cv::Point(x1, 0);
		}
		//point1 is fine
		else {
			point1 = cv::Point(0, y1);
		}



		//calculate point2 on the right side of the image
		int y2 = int(m * width + t);

		//point2 is above the image -> calculate point2 on the top of the image
		if (y2 > height) {
			double x2d = (height - t) / m;
			int x2 = (int)x2d;
			point2 = cv::Point(x2, height);
		}
		//point2 is below the image -> calculate point2 on the bottom of the image
		else if (y2 < 0) {
			double x2d = (0 - t) / m;
			int x2 = (int)x2d;
			point2 = cv::Point(x2, 0);
		}
		//point2 is fine
		else {
			point2 = cv::Point(width, y2);
		}

	}

	//test each contour point against the split line and add it to the corresponding split part
	for (int i = 0; i < contourPoints.size(); i++) {
		cv::Point testPoint = contourPoints.at(i);

		if (isLeftOf(testPoint, point1, point2)){
			splitPart1.push_back(testPoint);
		}
		else {
			splitPart2.push_back(testPoint);
		}
	}

	//rasterize the split line
	vector<cv::Point> splitLine = util::bresenhamLine(point1, point2);

	//test for each point of the splitLine if it is contained in the original contour
	for (int i = 0; i < splitLine.size(); i++) {
		cv::Point testPoint = splitLine.at(i);
		double val = cv::pointPolygonTest(contourPoints, testPoint, false);
		//point is inside or on the edge of the contour
		if (val >= 0) {
			//add the contained points of the split line to both splitParts
			splitPart1.push_back(testPoint);
			splitPart2.push_back(testPoint);
		}
	}

}












void getContours(string inputFile, string contourFile) {
	ifstream infile(inputFile);
	int width, height;
	string dummy;
	infile >> dummy >> width;
	infile >> dummy >> height;


	int* pixels = new int[height * width];
	for (int i = 0; i < height*width; i++) {
		pixels[i] = 0;
	}

	int x, y;

	while (infile >> x >> y) {
		pixels[x + y*width] = 100;
	}

	vector<int> reducedPixels = util::reducePixels(pixels, width, height, 1000, "test.png");


	ofstream outfile(contourFile);

	outfile << "w " << width << endl;
	outfile << "h " << height << endl;

	for (int i = 0; i < width*height; i++) {
		if (reducedPixels[i] != 0) {
			int y = i / width;
			int x = i % width;
			outfile << x << " " << y << endl;
		}
	}

}





void getContoursRonin(string inputFile, string contourFile) {
	ifstream infile(inputFile);
	int width, height;
	string dummy;
	infile >> dummy >> width;
	infile >> dummy >> height;


	int* pixels = new int[height * width];
	for (int i = 0; i < height*width; i++) {
		pixels[i] = 0;
	}

	int x, y;

	while (infile >> x >> y) {
		pixels[x + y*width] = 100;
	}

	vector<int> reducedPixels = util::reducePixels(pixels, width, height, 1000, "test.png");


	ofstream outfile(contourFile);

	outfile << "pixel" << endl;
	outfile << "list: 0" << endl;

	for (int i = 0; i < width*height; i++) {
		if (reducedPixels[i] != 0) {
			int y = i / width;
			int x = i % width;
			outfile << x << " " << y << endl;
		}
	}
	outfile << -1 << " " << -1 << endl;

}




