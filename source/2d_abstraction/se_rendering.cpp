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
#include "text_output.h"
#include "se_util.h"
#include "util.h"

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





template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}


namespace se {


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

	std::vector<std::tuple<double, double>> renderSuperellipseToPixelvector(double xc, double yc, double a, double b, double epsilon, double theta) {
		int points = 100;
		double step = 360.0 / points;
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

			std::tuple<double, double> point(xNew, yNew);
			pointList.emplace_back(point);
			//std::cout << "X: " << x << " Y: " << y << std::endl;
		}

		return pointList;
	}














	/// processes the superellipses given as an array by rendering them to a list of segments and writing them to an image
	/**
	*
	*/

	int processSuperellipsesFromArray(double* ellipses, std::string outputFile) {


		double xCenter;
		double yCenter;
		double a;
		double b;
		double epsilon;
		double theta;

		std::vector<std::tuple<double, double>> pointListOfEllipse;		//list of points rendered from one superellipse
		std::vector<std::vector<std::tuple<double, double>>> listOfEllipses;	//list of all pointListOfEllipses

		// reading superellipse parameters from textfile
		for (int i = 0; i < sizeof(ellipses) / 6; i++) {
			xCenter = ellipses[6 * i];
			yCenter = ellipses[6 * i + 1];
			theta = ellipses[6 * i + 2];
			a = ellipses[6 * i + 3];
			b = ellipses[6 * i + 4];
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

	int processSuperellipsesFromVector(std::vector<se::superellipse> superellipses, std::string outputFile, const int width, const int height) {

		//	double xc, yc, a, b, epsilon, theta;

		std::vector<std::tuple<double, double>> pointListOfEllipse;		//list of points rendered from one superellipse
		std::vector<std::vector<std::tuple<double, double>>> listOfEllipses;	//list of all pointListOfEllipses

		// reading superellipse parameters
		for (int i = 0; i < superellipses.size(); i++) {
			se::superellipse superellipse = superellipses.at(i);

			pointListOfEllipse = renderSuperellipseToPixelvector(superellipse.xc, superellipse.yc, superellipse.a, superellipse.b, superellipse.epsilon, superellipse.theta);
			listOfEllipses.emplace_back(pointListOfEllipse);
		}

		image_output::writeSuperellipsesToImage(listOfEllipses, outputFile + ".png", width, height);

		return 0;
	}



	int processSuperellipsesToBoundingBoxFromVector(std::vector<se::superellipse> superellipses, std::string outputName, const int width, const int height, const int offset) {

		std::vector<viral_core::vector2f> corners;
		std::vector<std::vector<viral_core::vector2f>> listOfBoundingBoxes;

		std::vector<int> cornerPixels;

		// reading superellipse parameters
		for (int i = 0; i < superellipses.size(); i++) {
			se::superellipse superellipse = superellipses.at(i);

			corners = getBoundingboxOfSuperellipse(superellipse.xc, superellipse.yc, superellipse.a, superellipse.b, superellipse.epsilon, superellipse.theta);
			listOfBoundingBoxes.emplace_back(corners);

			for (int j = 0; j < corners.size(); j++) {
				viral_core::vector2f corner = corners.at(j);
				int x = offset + (int)corner.x;
				int y = offset + (int)corner.y;

				cornerPixels.emplace_back(x);
				cornerPixels.emplace_back(y);
			}
		}

		image_output::pixelVectorToImage(cornerPixels, 640 + 2 * offset, 480 + 2 * offset, outputName + "_corners.png");
		text_output::writeIntVectorToTextfile(outputName + ".txt", cornerPixels);


		std::vector<int> lineData = util::createBoundingBoxLinesFromCorners(cornerPixels);


		image_output::renderArrayOfLinesToImage(outputName + "_lines.png", 640 + 2 * offset, 480 + 2 * offset, lineData);

		return 0;
	}

}