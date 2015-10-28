/**
*************************************************************************
*
* @file se_rendering.cpp
*
* Output class for rendering superellipses from text files to images, implementation.
*
************************************************************************/

#include "se_rendering.h"
#include "image_output.h"
#include "se_util.h"

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



/// evaluates the superellipse by calculaten the x and y coordinates of the border of the superellipse at the given angle phi
/**
*
*/

void evaluateSuperellipse(double a, double b, double epsilon, double phi, double *x, double *y) {

	double cost = cos(phi);
	double sint = sin(phi);
	*x = a*sgn<double>(cost)* pow(abs(cost), epsilon);
	*y = b*sgn<double>(sint)* pow(abs(sint), epsilon);

}




/// renders the superellipse given by its parameters to a list of 100 points forming segments on the border of the superellipse
/**
*
*/

vector<tuple<double, double>> renderSuperellipseToPixelvector(double xc, double yc, double a, double b, double epsilon, double theta) {
	int points = 100;
	double step = 360.0/points;
	double halfPi = M_PI*2.0;
	std::vector<std::tuple<double, double>> pointList;

	for (int i = 0; i < points; i++) {
		double angle = step*i;
		double t = 1.0 / 360.0*angle;
		double phi = t * halfPi;
		double x;
		double y;
		evaluateSuperellipse(a, b, epsilon, phi, &x, &y);
		
		//include offset and rotation of the superellipse

		//rotate with extTheta:
		double xNew = cos(theta)*x - sin(theta)*y;
		double yNew = sin(theta)*x + cos(theta)*y;
		
		//include offset:
		xNew += xc;
		yNew += yc;
		
		std::tuple<double, double> point (xNew, yNew);
		pointList.emplace_back(point);
		//std::cout << "X: " << x << " Y: " << y << std::endl;
	}

	return pointList;
}





/// calculates the bounding box of the given superellipse, returns the corners in counter clockwise order
/**
*
*/

vector<tuple<double, double>> getBoundingboxOfSuperellipse(double xc, double yc, double a, double b, double epsilon, double theta) {

	double halfPi = M_PI*0.5;

	std::vector<std::tuple<double, double>> corners;

	double xa = a * cos(theta);
	double ya = a * sin(theta);
	double xb = b * cos(theta + halfPi);
	double yb = b * sin(theta + halfPi);

	double cornerX;
	double cornerY;


	//corner "top right"
	cornerX = xc + xa + xb;
	cornerY = yc + ya + yb;
	corners.emplace_back(std::tuple<double, double>(cornerX, cornerY));

	//corner "top right"
	cornerX = xc - xa + xb;
	cornerY = yc - ya + yb;
	corners.emplace_back(std::tuple<double, double>(cornerX, cornerY));

	//corner "top right"
	cornerX = xc - xa - xb;
	cornerY = yc - ya - yb;
	corners.emplace_back(std::tuple<double, double>(cornerX, cornerY));

	//corner "top right"
	cornerX = xc + xa - xb;
	cornerY = yc + ya - yb;
	corners.emplace_back(std::tuple<double, double>(cornerX, cornerY));


	return corners;
}




vector<int> createBoundingBoxLinesFromCorners(vector<int> cornerPixels) {
	vector<int> lineData;
	int nrOfBoxes = cornerPixels.size() / 8;
	int nrOfLines = nrOfBoxes * 4;

	for (int i = 0; i < nrOfBoxes; i++) {
		int x1 = cornerPixels.at(8 * i);
		int y1 = cornerPixels.at(8 * i + 1);
		int x2 = cornerPixels.at(8 * i + 2);
		int y2 = cornerPixels.at(8 * i + 3);
		int x3 = cornerPixels.at(8 * i + 4);
		int y3 = cornerPixels.at(8 * i + 5);
		int x4 = cornerPixels.at(8 * i + 6);
		int y4 = cornerPixels.at(8 * i + 7);

		lineData.emplace_back(x1);
		lineData.emplace_back(y1);
		lineData.emplace_back(x2);
		lineData.emplace_back(y2);

		lineData.emplace_back(x2);
		lineData.emplace_back(y2);
		lineData.emplace_back(x3);
		lineData.emplace_back(y3);

		lineData.emplace_back(x3);
		lineData.emplace_back(y3);
		lineData.emplace_back(x4);
		lineData.emplace_back(y4);

		lineData.emplace_back(x4);
		lineData.emplace_back(y4);
		lineData.emplace_back(x1);
		lineData.emplace_back(y1);
	}

	return lineData;

}






/// processes the superellipses given as an array by rendering them to a list of segments and writing them to an image
/**
*
*/

int processSuperellipsesFromArray(double* ellipses, string outputFile) {


	double xCenter;
	double yCenter;
	double a;
	double b;
	double epsilon;
	double theta;

	vector<tuple<double, double>> pointListOfEllipse;		//list of points rendered from one superellipse
	vector<vector<tuple<double, double>>> listOfEllipses;	//list of all pointListOfEllipses

	// reading superellipse parameters from textfile
	for (int i = 0; i < sizeof(ellipses) / 6; i++) {
		xCenter = ellipses[6 * i];
		yCenter = ellipses[6 * i + 1];
		theta =	  ellipses[6 * i + 2];
		a =       ellipses[6 * i + 3];
		b =       ellipses[6 * i + 4];
		epsilon = ellipses[6 * i + 5];

		pointListOfEllipse = renderSuperellipseToPixelvector(xCenter, yCenter, a, b, epsilon, theta);
		listOfEllipses.emplace_back(pointListOfEllipse);
	}

	image_output::writeSuperellipsesToImage(listOfEllipses, outputFile, 1000, 1000);

	return 0;
}





/// processes the superellipses given as a vector by rendering them to a list of segments and writing them to an image
/**
*
*/

int processSuperellipsesFromVector(vector<se::superellipse> superellipses, string outputFile, const int width, const int height) {

//	double xc, yc, a, b, epsilon, theta;

	vector<tuple<double, double>> pointListOfEllipse;		//list of points rendered from one superellipse
	vector<vector<tuple<double, double>>> listOfEllipses;	//list of all pointListOfEllipses

	// reading superellipse parameters
	for (int i = 0; i < superellipses.size(); i++) {
		se::superellipse superellipse = superellipses.at(i);

		pointListOfEllipse = renderSuperellipseToPixelvector(superellipse.xc, superellipse.yc, superellipse.a, superellipse.b, superellipse.epsilon, superellipse.theta);
		listOfEllipses.emplace_back(pointListOfEllipse);
	}

	image_output::writeSuperellipsesToImage(listOfEllipses, outputFile, width, height);

	return 0;
}



int processSuperellipsesToBoundingBoxFromVector(vector<se::superellipse> superellipses, string outputFile, const int width, const int height) {

	vector<tuple<double, double>> corners;
	vector<vector<tuple<double, double>>> listOfBoundingBoxes;

	vector<int> cornerPixels;

	// reading superellipse parameters
	for (int i = 0; i < superellipses.size(); i++) {
		se::superellipse superellipse = superellipses.at(i);

		corners = getBoundingboxOfSuperellipse(superellipse.xc, superellipse.yc, superellipse.a, superellipse.b, superellipse.epsilon, superellipse.theta);
		listOfBoundingBoxes.emplace_back(corners);

		for (int j = 0; j < corners.size(); j++) {
			tuple<double, double> corner = corners.at(j);
			int x = (int)std::get<0>(corner);
			int y = (int)std::get<1>(corner);

			cornerPixels.emplace_back(x);
			cornerPixels.emplace_back(y);
		}
	}

	image_output::pixelVectorToImage(cornerPixels, 640, 480, "test_" + outputFile);

	vector<int> lineData = createBoundingBoxLinesFromCorners(cornerPixels);
	image_output::renderArrayOfLinesToImage("test2_" + outputFile, 640, 480, lineData);

	return 0;
}