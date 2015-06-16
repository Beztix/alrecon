/**
*************************************************************************
*
* @file se_rosin_recursive.cpp
*
* This file contains all methods to use the superellipse fitting with the method provided by Paul Rosin to fit superellipses recursively
* to given contours.
*
* 
*
************************************************************************/

#include <vector>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "se_rosin_core.h"
#include "se_split.h"
#include "util.h"
#include "se_util.h"
#include "image_output.h"
#include "blob.h"
#include "image_input.h"

#include <Windows.h>

using namespace std;





//#define DEBUG

/// uses the rosin fitting of superellipses in a recursive way to fit multiple superellipses to a contour until a certain quality level is reached
/**
* This method recursively fits superellipse a superellipse to a given contour, returning the fitted superellipse if the fit is of high quality
* and splitting the input to fit two separate superellipses if the fit by one superellipse is of low quality. The splitting process is followed by recursive calls,
* so the contour is splitted until the fits achieve the desired quality.
*/

vector<se::superellipse> useRosinRecursive(vector<cv::Point> contourPoints, int offsetX, int offsetY, const int width, const int height, const int qualityValue) {
	
	//fit one ellipse to the contour
	vector<int> contourVector;
	for (int j = 0; j < contourPoints.size(); j++) {
		contourVector.push_back(contourPoints.at(j).x);
		contourVector.push_back(contourPoints.at(j).y);
	}
	int no_contourPixels = int(contourVector.size()) / 2;
	int* contourList = &contourVector[0];

	double xc, yc, theta, a, b, epsilon;
	fitEllipseRosin2(contourList, no_contourPixels, &xc, &yc, &theta, &a, &b, &epsilon);

	#ifdef DEBUG
	image_output::pixelVectorToImage(contourVector, width, height, "initialContour.png");
	#endif

	//make "a" the larger axis of the fitted superellipse
	if (a < b) {
		double temp = a;
		a = b;
		b = temp;
		theta += 1.57079;
	}

	#ifdef DEBUG
	image_output::renderSuperEllipseToImage("initialEllipse.png", width, height, xc, yc, theta, a, b, epsilon);
	#endif

	//evaluate the fit
	double currentQualityValue = se::evaluateFitEuclidean(contourVector, xc, yc, theta, a, b, epsilon);


	//===========================================================
	//==    fit is not good enough yet: proceed recursively:   ==
	//===========================================================
	if (currentQualityValue > qualityValue) {

		//split the pixel grid
		vector<cv::Point> part1;
		vector<cv::Point> part2;
		splitContourToVectors(contourPoints, width, height, xc, yc, theta, part1, part2);

		#ifdef DEBUG
		image_output::renderContourColored("originalContour.png", width, height, contourPoints);
		image_output::renderContourColored("contourPart1.png", width, height, part1);
		image_output::renderContourColored("contourPart2.png", width, height, part2);
		#endif

		// --------------------------
		// ---        part 1       --
		// --------------------------
		cv::Mat part1Mat = cv::Mat(height, width, CV_8UC1);
		part1Mat.setTo(0);
		for (int i = 0; i < part1.size(); i++) {
			//add the points of part1 to the Mat
			cv::Point currentPoint = part1.at(i);
			int x = currentPoint.x - 0;
			int y = currentPoint.y - 0;
			part1Mat.at<uchar>(y, x) = 1;
		}

		//find the contours in part1
		vector<vector<cv::Point>> contours1;
		cv::findContours(part1Mat, contours1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

		vector<se::superellipse> totalResultVector1;
		//for each contour (equals blob)
		for (int i = 0; i < contours1.size(); i++) {
			vector<cv::Point> currentContour = contours1.at(i);
			//image_output::renderContourColored("part1_" + to_string(i) + ".png", width, height, currentContour);


			//threshold: if contour is too small don't fit to the contour (just ignore)
			if (currentContour.size() < 5) {
				//TODO: HOW TO DEAL WITH THIS??
				continue;
			}

			//determine dimension and offset of the contour
			int minX = INT_MAX;
			int maxX = INT_MIN;
			int minY = INT_MAX;
			int maxY = INT_MIN;
			for (int j = 0; j < currentContour.size(); j++) {
				cv::Point currentPoint = currentContour.at(j);
				if (currentPoint.x < minX) minX = currentPoint.x;
				if (currentPoint.x > maxX) maxX = currentPoint.x;
				if (currentPoint.y < minY) minY = currentPoint.y;
				if (currentPoint.y > maxY) maxY = currentPoint.y;
			}
			int sizeX = maxX - minX + 3;
			int sizeY = maxY - minY + 3;
			int newOffsetX = minX - 1;
			int newOffsetY = minY - 1;

			//substract the offset from the contour points
			vector<cv::Point> currentContourWithOffset(currentContour.size());
			for (int j = 0; j < currentContour.size(); j++) {
				cv::Point currentPoint = currentContour.at(j);
				cv::Point newPoint = cv::Point(currentPoint.x - newOffsetX, currentPoint.y - newOffsetY);
				currentContourWithOffset.at(j) = newPoint;
			}
			//image_output::renderContourColored("part1_" + to_string(i) + "_offset.png", sizeX, sizeY, currentContourWithOffset);

			//use rosin recursive on the new contour
			vector<se::superellipse> resultVector1 = useRosinRecursive(currentContourWithOffset, newOffsetX, newOffsetY, sizeX, sizeY, qualityValue);
			
			//add the offset to the calculated ellipses
			for (int j = 0; j < resultVector1.size(); j++) {
				resultVector1[j].xc += offsetX;
				resultVector1[j].yc += offsetY;
			}
			totalResultVector1.insert(totalResultVector1.end(), resultVector1.begin(), resultVector1.end());
		}
		// ------- end of part 1 -------


		// --------------------------
		// ---        part 2       --
		// --------------------------

		cv::Mat part2Mat = cv::Mat(height, width, CV_8UC1);
		part2Mat.setTo(0);
		for (int i = 0; i < part2.size(); i++) {
			//add the points of part1 to the Mat
			cv::Point currentPoint = part2.at(i);
			int x = currentPoint.x - 0;
			int y = currentPoint.y - 0;
			part2Mat.at<uchar>(y, x) = 1;
		}

		//find the contours in part2
		vector<vector<cv::Point>> contours2;
		cv::findContours(part2Mat, contours2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

		vector<se::superellipse> totalResultVector2;
		//for each contour (equals blob)
		for (int i = 0; i < contours2.size(); i++) {
			vector<cv::Point> currentContour = contours2.at(i);

			//threshold: if contour is too small don't fit to the contour (just ignore)
			if (currentContour.size() < 5) {
				//TODO: HOW TO DEAL WITH THIS??
				continue;
			}

			//determine dimension and offset of the contour
			int minX = INT_MAX;
			int maxX = INT_MIN;
			int minY = INT_MAX;
			int maxY = INT_MIN;
			for (int j = 0; j < currentContour.size(); j++) {
				cv::Point currentPoint = currentContour.at(j);
				if (currentPoint.x < minX) minX = currentPoint.x;
				if (currentPoint.x > maxX) maxX = currentPoint.x;
				if (currentPoint.y < minY) minY = currentPoint.y;
				if (currentPoint.y > maxY) maxY = currentPoint.y;
			}
			int sizeX = maxX - minX + 3;
			int sizeY = maxY - minY + 3;
			int newOffsetX = minX - 1;
			int newOffsetY = minY - 1;

			//substract the offset from the contour points
			vector<cv::Point> currentContourWithOffset(currentContour.size());
			for (int j = 0; j < currentContour.size(); j++) {
				cv::Point currentPoint = currentContour.at(j);
				cv::Point newPoint = cv::Point(currentPoint.x - newOffsetX, currentPoint.y - newOffsetY);
				currentContourWithOffset.at(j) = newPoint;
			}

			//use rosin recursive on the new contour
			vector<se::superellipse> resultVector2 = useRosinRecursive(currentContourWithOffset, newOffsetX, newOffsetY, sizeX, sizeY, qualityValue);

			//add the offset to the calculated ellipses
			for (int j = 0; j < resultVector2.size(); j++) {
				resultVector2[j].xc += offsetX;
				resultVector2[j].yc += offsetY;
			}
			totalResultVector2.insert(totalResultVector2.end(), resultVector2.begin(), resultVector2.end());
		}
		// ------- end of part 2 -------
	

		//combine the fitted ellipses from the recursive calls and return them
		totalResultVector1.insert(totalResultVector1.end(), totalResultVector2.begin(), totalResultVector2.end());
		return std::move(totalResultVector1);
	}


	//=====================================
	//==    fit is already good enough   ==
	//=====================================
	else {
		//test if the fit is already conservative
		bool conservative = se::isFitConservative(contourVector, xc, yc, theta, a, b, epsilon);
		
		//if not conservative: enlarge the superellipse until it is conservative
		while (!conservative) {
			a += 0.5;
			b += 0.5;
			if (epsilon > 2) {
				epsilon -= 0.3;
			}
			conservative = se::isFitConservative(contourVector, xc, yc, theta, a, b, epsilon);
		}

		//return the fitted ellipse
		
		se::superellipse fittedSuperellipse(xc + offsetX, yc + offsetY, theta, a, b, epsilon, currentQualityValue);
		
		vector<se::superellipse> resultVector;
		resultVector.emplace_back(fittedSuperellipse);
		return std::move(resultVector);
	}
	
}







/// this method seperates the given contours and resizes the image to the contour dimensions, then calls useRosinRecursive() on each contour 
/**
* This method is used to start the recursive fitting of superellipses to the given contours.
* It clips the surrounding image of each contour to the contour dimension and calculates the corresponding offset, to achieve a significant performance optimization.
* Then useRosonRecursive() is used to recursively fit superellipse to each of the contours.
* All of the fitted superellipses are returned together.
*/

vector<se::superellipse> startRosinRecursive(vector<vector<cv::Point>> contours, int &width, int &height, int quality) {
	vector<se::superellipse> totalEllipsesVector;

	//for each contour:
	for (int i = 0; i < contours.size(); i++) {
		vector<cv::Point> currentContour = contours.at(i);

		//determine dimension and offset of the contour
		int minX = INT_MAX;
		int maxX = INT_MIN;
		int minY = INT_MAX;
		int maxY = INT_MIN;
		for (int j = 0; j < currentContour.size(); j++) {
			cv::Point currentPoint = currentContour.at(j);
			if (currentPoint.x < minX) minX = currentPoint.x;
			if (currentPoint.x > maxX) maxX = currentPoint.x;
			if (currentPoint.y < minY) minY = currentPoint.y;
			if (currentPoint.y > maxY) maxY = currentPoint.y;
		}
		int sizeX = maxX - minX + 3;
		int sizeY = maxY - minY + 3;
		int offsetX = minX - 1;
		int offsetY = minY - 1;


		//substract the offset from the contour points
		vector<cv::Point> currentContourWithOffset(currentContour.size());
		for (int j = 0; j < currentContour.size(); j++) {
			cv::Point currentPoint = currentContour.at(j);
			cv::Point newPoint = cv::Point(currentPoint.x - offsetX, currentPoint.y - offsetY);
			currentContourWithOffset.at(j) = newPoint;
		}



		//use recursive Rosin fitting to fit superellipses to the contour
		vector<se::superellipse> contourEllipsesVector = useRosinRecursive(currentContourWithOffset, offsetX, offsetY, sizeX, sizeY, quality);


		//draw the fitted superellipses
		//int err = processSuperellipsesFromVector(contourEllipsesVector, to_string(i) + ellipseImage, width, height);


		//add the superellipses of this contour to the vector of all superellipses
		totalEllipsesVector.insert(totalEllipsesVector.end(), contourEllipsesVector.begin(), contourEllipsesVector.end());
	}
	
	return totalEllipsesVector;



}