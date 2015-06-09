/**
*************************************************************************
*
* @file se_ronin_recursive.cpp
*
* 
*
************************************************************************/

#include <vector>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "se_ronin_core.h"
#include "se_split.h"
#include "util.h"
#include "image_output.h"
#include "blob.h"
#include "image_input.h"

using namespace std;




/// evaluates the quality of a superellipsefit by returning a fitValue, the lower the better
/**
* This method uses the contour and the parameters of the superellipse fitted to that contour to calculate a quality of the fit (fitValue).
* The fitValue is calculated as the sum of the values received by putting the points of the contourn in the implicid superellipse equation
*/

double evaluateFitImplicid(vector<int> contourVector, double xc, double yc, double theta, double a, double b, double epsilon) {
	int no_contourPixels = int(contourVector.size()) / 2;
	int* contourList = &contourVector[0];
	double cosine2 = cos(-theta);
	double sine2 = sin(-theta);
	double tx, ty;
	double x, y, x2, y2, qualityValue;
	double totalValue = 0;

	double exponent = 2.0 / epsilon;

	//for each point of the contour:
	for (int i = 0; i < no_contourPixels; i++){
		x = contourList[i * 2];
		y = contourList[i * 2 + 1];
		
		//transform coordinates in the coordinatesystem of the superellipse
		tx = x - xc;
		ty = y - yc;
		x2 = (tx * cosine2 - ty * sine2);
		y2 = (tx * sine2 + ty * cosine2);

		//calculate the value of the point in the superellipse equation
		double firstTerm = (fabs(x2) / a);
		double secondTerm = (fabs(y2) / b);
		double equationValue = pow(firstTerm, exponent) + pow(secondTerm, exponent) - 1.0;

		//if the value is < 0, so the point is on the inside of the superellipse:
		if (equationValue < 0) {
			//the value is close to -1, to receive a useful quality value add 1 and invert:
			double temp = 1.0001 + equationValue;	//1.0001 instead of 1 so that the inverted value doesnt get incredibly big
			qualityValue = 1 / temp;
		}
		//if the value is > 0, so the point is on the outside of the superellipse:
		else {
			//the value is already a useful quality value
			qualityValue = equationValue;
		}
		
		totalValue += qualityValue;
	}


	//cout << "Total Value: " << totalValue << endl;
	return totalValue;

}
















double evaluateFitEuclidean(vector<int> contourVector, double xc, double yc, double theta, double a, double b, double epsilon) {
	int no_contourPixels = int(contourVector.size()) / 2;
	const int* contourList = &contourVector[0];
	double cosine2 = cos(-theta);
	double sine2 = sin(-theta);
	double x, y, xTrans, yTrans, xRel, yRel;
	double totalOutDist = 0;
	double totalInDist = 0;

	//calculations not depending on x, y
	double exponent = 2.0 / epsilon;
	double exponentInv = epsilon / 2.0;
	double termOne = 1 / pow(a, exponent);

	//calculate a stepsize
	int stepsize;
	if (no_contourPixels < 20) {
		stepsize = 1;
	}
	else if (no_contourPixels > 400) {
		stepsize = 20;
	}
	else {
		double stepsizeD = no_contourPixels / 20;
		stepsize = (int)stepsizeD;
	}

	//for each point of the contour:
	for (int i = 0; i < no_contourPixels; i+=stepsize){
		x = contourList[i * 2];
		y = contourList[i * 2 + 1];

		//transform coordinates to the coordinatesystem of the superellipse
		xTrans = x - xc;
		yTrans = y - yc;
		xRel = (xTrans * cosine2 - yTrans * sine2);
		yRel = (xTrans * sine2 + yTrans * cosine2);

		//evaluate only in first quadrant
		xRel = fabs(xRel);
		yRel = fabs(yRel);

		
		//calculate the euclidean distance between the point and the point (xs, ys) on the superellipse
		double divOne = termOne + fabs(pow((yRel / (xRel*b)), exponent));
		double xs = pow((fabs(1 / (divOne))), exponentInv);
		double ys = xs * (yRel / xRel);
		double dist = (xRel - xs) * (xRel - xs) + (yRel - ys) * (yRel - ys);

		//special numerical case: xRel near 0, calculation might result in dist INF
		if (xRel < 0.00001) {
			dist = fabs(b - yRel);
			if (yRel < b) { //point on the inside of the superellipse
				xs = xRel + 1;
			}
			else { //point on the outside of the superellipse
				xs = xRel - 1;
			}
		}
		
		if (isinf(dist)) {
			double temp = 5.0;
		}


		//test if point is inside or outside of the superellipse (everything evaluated in first quadrant!)
		if (xs < xRel) { 
			totalOutDist += dist;
		}
		else {
			totalInDist += dist;
		}

	}

	//calculate the stepsize back into the value
	totalInDist *= stepsize;
	totalOutDist *= stepsize;

	return totalInDist + totalOutDist;

}




bool isFitConservative(vector<int> contourVector, double xc, double yc, double theta, double a, double b, double epsilon){
	int no_contourPixels = int(contourVector.size()) / 2;
	int* contourList = &contourVector[0];
	double cosine2 = cos(-theta);
	double sine2 = sin(-theta);
	double x, y, xTrans, yTrans, xRel, yRel;
	double totalOutDist = 0;
	double totalInDist = 0;

	//caclulations not depending on x, y
	double exponent = 2.0 / epsilon;
	double exponentInv = epsilon / 2.0;
	double termOne = 1 / pow(a, exponent);

	//for each point of the contour:
	for (int i = 0; i < no_contourPixels; i++){
		x = contourList[i * 2];
		y = contourList[i * 2 + 1];

		//transform coordinates to the coordinatesystem of the superellipse
		xTrans = x - xc;
		yTrans = y - yc;
		xRel = (xTrans * cosine2 - yTrans * sine2);
		yRel = (xTrans * sine2 + yTrans * cosine2);

		//evaluate only in first quadrant
		xRel = fabs(xRel);
		yRel = fabs(yRel);

		//calculate the euclidean distance between the point and the point (xs, ys) on the superellipse
		double divOne = termOne + fabs(pow((yRel / (xRel*b)), exponent));
		double xs = pow((fabs(1 / (divOne))), exponentInv);
		
		//special numerical case: xRel near 0, calculation might result in dist INF
		if (xRel < 0.000001) {
			if (yRel < b) { //point on the inside of the superellipse
				xs = xRel + 1;
			}
			else { //point on the outside of the superellipse
				xs = xRel - 1;
			}
		}


		//test if point is inside or outside of the superellipse (everything evaluated in first quadrant!)
		if (xs < xRel) {
			//xRel outside of the superellipse -> NOT CONSERVATIVE
			return false;
		}
	}

	//all points inside of the superellipse -> CONSERVATIVE
	return true;

}







//#define DEBUG

/// uses the ronin fitting of superellipses in a recursive way to fit mutliple superellipses to a contour until a certain quality level is reached
/**
* This method is called to recursively fit superellipses to a given contour, returning the fitted superellipse if the fit is of high quality
* and splitting the input to fit two separate superellipses if the fit by one superellipse is of low quality.
*/

vector<vector<double>> useRoninRecursive(vector<cv::Point> contourPoints, int offsetX, int offsetY, const int width, const int height, const int qualityValue) {
	
	//fit one ellipse to the contour
	vector<int> contourVector;
	for (int j = 0; j < contourPoints.size(); j++) {
		contourVector.push_back(contourPoints.at(j).x);
		contourVector.push_back(contourPoints.at(j).y);
	}
	int no_contourPixels = int(contourVector.size()) / 2;
	int* contourList = &contourVector[0];

	double xc, yc, theta, a, b, epsilon;
	fitEllipseRonin2(contourList, no_contourPixels, &xc, &yc, &theta, &a, &b, &epsilon);
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
	double currentValue = evaluateFitEuclidean(contourVector, xc, yc, theta, a, b, epsilon);


	//===========================================================
	//==    fit is not good enough yet: proceed recursively:   ==
	//===========================================================
	if (currentValue > qualityValue) {

		//split the pixel grid
		vector<cv::Point> part1;
		vector<cv::Point> part2;
		splitContourToVectors(contourPoints, width, height, xc, yc, theta, part1, part2);

		//image_output::renderContourColored("originalContour.png", width, height, contourPoints);
		
		//image_output::renderContourColored("part1.png", width, height, part1);
		//image_output::renderContourColored("part2.png", width, height, part2);


		// ========= part 1 =========

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

		vector<vector<double>> totalResultVector1;
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

			//use ronin recursive on the new contour
			vector<vector<double>> resultVector1 = useRoninRecursive(currentContourWithOffset, newOffsetX, newOffsetY, sizeX, sizeY, qualityValue);
			
			//add the offset to the calculated ellipses
			for (int j = 0; j < resultVector1.size(); j++) {
				resultVector1[j][0] = resultVector1[j][0] + offsetX;
				resultVector1[j][1] = resultVector1[j][1] + offsetY;
			}
			totalResultVector1.insert(totalResultVector1.end(), resultVector1.begin(), resultVector1.end());
		}
		// ===== end of part 1 =====


		// ========= part 2 =========

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

		vector<vector<double>> totalResultVector2;
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

			//use ronin recursive on the new contour
			vector<vector<double>> resultVector2 = useRoninRecursive(currentContourWithOffset, newOffsetX, newOffsetY, sizeX, sizeY, qualityValue);

			//add the offset to the calculated ellipses
			for (int j = 0; j < resultVector2.size(); j++) {
				resultVector2[j][0] = resultVector2[j][0] + offsetX;
				resultVector2[j][1] = resultVector2[j][1] + offsetY;
			}
			totalResultVector2.insert(totalResultVector2.end(), resultVector2.begin(), resultVector2.end());
		}
		// ===== end of part 2 =====
	
		//combine the fitted ellipses from the recursive calls and return them
		totalResultVector1.insert(totalResultVector1.end(), totalResultVector2.begin(), totalResultVector2.end());
		return std::move(totalResultVector1);
	}


	//=====================================
	//==    fit is already good enough   ==
	//=====================================
	else {
		//test if the fit is already conservative
		bool conservative = isFitConservative(contourVector, xc, yc, theta, a, b, epsilon);
		
		//if not conservative: enlarge the superellipse until it is conservative
		while (!conservative) {
			a += 0.5;
			b += 0.5;
			if (epsilon > 2) {
				epsilon -= 0.3;
			}
			conservative = isFitConservative(contourVector, xc, yc, theta, a, b, epsilon);
		}

		//return the fitted ellipse
		vector<double> fittedSuperellipse = { xc + offsetX, yc + offsetY, theta, a, b, epsilon };
		vector<vector<double>> resultVector;
		resultVector.emplace_back(fittedSuperellipse);
		return std::move(resultVector);
	}
	
}
