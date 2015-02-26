/**
*************************************************************************
*
* @file se_rendering.cpp
*
* Output class for rendering superellipses from text files to images, implementation.
*
************************************************************************/

#include "se_rendering.h"

#define _USE_MATH_DEFINES

#include <list>
#include <math.h>
#include <tuple>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;


template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}


void evaluateSuperellipse(double a, double b, double epsilon, double theta, double *x, double *y) {

	double cost = cos(theta);
	double sint = sin(theta);
	*x = a*sgn<double>(cost)* pow(abs(cost), epsilon);
	*y = b*sgn<double>(sint)* pow(abs(sint), epsilon);

}



void writeSuperellipsesToImage(vector<vector<tuple<double, double>>> pointListContainer, string imgName) {
	int offsetX = 200;
	int offsetY = 200;
	int width = 600;
	int height = 600;

	Mat image = Mat(height, width, CV_8UC3);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			image.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 0);
		}
	}


	//go through all superellipses
	for (vector<vector<tuple<double, double>>>::iterator iterator = pointListContainer.begin(); iterator != pointListContainer.end(); iterator++) {
		vector<tuple<double, double>> pointList = *iterator;

		//for each superellipse:

		Point previousPoint;
		Point firstPoint;
		Point currentPoint;
		bool first = true;

		//go through all rendered points
		for (vector<tuple<double, double>>::iterator it = pointList.begin(); it != pointList.end(); it++) {
			tuple<double, double> point = *it;
			double x = get<0>(point);
			double y = get<1>(point);
			x += offsetX;
			y += offsetY;
			x += 0.5;		//rounding instead of cutting off by casting to int
			y += 0.5;
			int xInt = static_cast<int>(x);
			int yInt = static_cast<int>(y);
			currentPoint = Point(xInt, yInt);

			if (!first) {
				line(image, previousPoint, currentPoint, Scalar(255, 200, 20), 1, 8, 0);
			}
			else {
				firstPoint = currentPoint;
			}

			previousPoint = currentPoint;
			first = false;
		}
		line(image, currentPoint, firstPoint, Scalar(255, 200, 20), 1, 8, 0);

	}


	imwrite(imgName, image);


}





vector<tuple<double, double>> renderSuperellipse(double xc, double yc, double a, double b, double epsilon, double extTheta) {
	int points = 100;
	double step = 360.0/points;
	double halfPi = M_PI*2.0;
	std::vector<std::tuple<double, double>> pointList;

	for (int i = 0; i < points; i++) {
		double angle = step*i;
		double t = 1.0 / 360.0*angle;
		double theta = t * halfPi;
		double x;
		double y;
		evaluateSuperellipse(a, b, epsilon, theta, &x, &y);
		
		//include offset and rotation of the superellipse

		//rotate with extTheta:
		double xNew = cos(extTheta)*x - sin(extTheta)*y;
		double yNew = sin(extTheta)*x + cos(extTheta)*y;
		
		//include offset:
		xNew += xc;
		yNew += yc;
		
		std::tuple<double, double> point (xNew, yNew);
		pointList.emplace_back(point);
		std::cout << "X: " << x << " Y: " << y << std::endl;
	}





	return pointList;


}




int processSuperellipsesFromTextfile(string input, string output) {
	
	ifstream infile(input);

	string line;
	int i;
	
 	// reading the first two lines (not needed here)
	infile >> line;
	cout << line << endl;
	infile >> line >> i;
	cout << line << i << endl;

	
	
	double d;
	double xCenter;
	double yCenter;
	double a;
	double b;
	double epsilon;
	double theta;

	vector<tuple<double, double>> pointListOfEllipse;		//list of points rendered from one superellipse
	vector<vector<tuple<double, double>>> listOfEllipses;	//list of all pointListOfEllipses
	
	// reading superellipse parameters from textfile
	while (infile >> line >> d >> xCenter >> yCenter >> d >> d >> d >> d >> a >> b >> epsilon >> theta >> d) {
		cout << xCenter << " " << yCenter << " " << a << " " << b << " " << epsilon << " " << theta << endl;
		pointListOfEllipse = renderSuperellipse(xCenter, yCenter, a, b, epsilon, theta);
		listOfEllipses.emplace_back(pointListOfEllipse);
	}

	writeSuperellipsesToImage(listOfEllipses, output);

	
	return 0;

}