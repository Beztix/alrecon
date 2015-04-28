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

	//caclulations not depending on x, y
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
		if (xRel < 0.000001) {
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

vector<vector<double>> useRoninRecursive(vector<int> pixelGrid, vector<int> contourVector, const int width, const int height, const int qualityValue) {

	//fit one ellipse to the contour in the contourVector
	int no_contourPixels = int(contourVector.size()) / 2;
	int* contourList = &contourVector[0];

	#ifdef DEBUG
	image_output::pixelVectorToImage(contourVector, width, height, "inititalContour.png");
	#endif

	double xc, yc, theta, a, b, epsilon;

	fitEllipseRonin2(contourList, no_contourPixels, &xc, &yc, &theta, &a, &b, &epsilon);


	//make a the larger axis of the superellipse
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

	//if the fit is not good enough yet, proceed recursively:
	if (currentValue > qualityValue) {

		//split the pixel grid
		cv::Mat part1;
		cv::Mat part2;
		splitImageToMats(pixelGrid, width, height, xc, yc, theta, part1, part2);
		#ifdef DEBUG
		imwrite("part1.png", part1);
		imwrite("part2.png", part2);
		#endif


		//find the connected components in split part 1
		cv::Mat part1gray;
		cvtColor(part1, part1gray, CV_BGR2GRAY);
		cv::Mat binaryPart1;
		threshold(part1gray, binaryPart1, 0.0, 1.0, cv::THRESH_BINARY);
		vector<vector<cv::Point2i>> blobsPart1;
		blob::FindBlobs(binaryPart1, blobsPart1);

		vector<vector<double>> totalResultVector1;

		//for each connected component:
		for (int i = 0; i < blobsPart1.size(); i++) {
			vector<cv::Point2i> currentBlob = blobsPart1.at(i);

			//threshold: if blob is too small don't fit to the blob (just ignore)
			if (currentBlob.size() < 5) {
				continue;
			}

			//create a pixelGrid from the blob points
			vector<int> splitPixelGrid1 (width*height);
			std::fill(splitPixelGrid1.begin(), splitPixelGrid1.end(), 0);
			for (int j = 0; j < currentBlob.size(); j++) {
				//add the points of the connected component to the pixelGrid
				cv::Point2i currentPoint = currentBlob.at(j);
				int x = currentPoint.x;
				int y = currentPoint.y;
				splitPixelGrid1[y*width + x] = 200;
			}

			#ifdef DEBUG
			image_output::pixelGridToImage(splitPixelGrid1, width, height, "blobpart1_" + to_string(i) + ".png");
			#endif

			//get the contour from the connected component
			vector<int> contourVector1 = util::getContourVector(splitPixelGrid1, width, height);

			#ifdef DEBUG
			image_output::pixelVectorToImage(contourVector1, width, height, "contour1_" + to_string(i) + ".png");
			#endif

			//use ronin recursive on the new contour
			vector<vector<double>> resultVector1 = useRoninRecursive(splitPixelGrid1, contourVector1, width, height, qualityValue);
			totalResultVector1.insert(totalResultVector1.end(), resultVector1.begin(), resultVector1.end());
		}


		//find the connected components in split part 1
		cv::Mat part2gray;
		cvtColor(part2, part2gray, CV_BGR2GRAY);
		cv::Mat binaryPart2;
		threshold(part2gray, binaryPart2, 0.0, 1.0, cv::THRESH_BINARY);
		vector<vector<cv::Point2i>> blobsPart2;
		blob::FindBlobs(binaryPart2, blobsPart2);

		vector<vector<double>> totalResultVector2;

		//for each connected component:
		for (int i = 0; i < blobsPart2.size(); i++) {
			vector<cv::Point2i> currentBlob = blobsPart2.at(i);

			//threshold: if blob is too small don't fit to the blob (just ignore)
			if (currentBlob.size() < 5) {
				continue;
			}

			//create a pixelGrid from the blob points
			vector<int> splitPixelGrid2(width*height);
			std::fill(splitPixelGrid2.begin(), splitPixelGrid2.end(), 0);
			for (int j = 0; j < currentBlob.size(); j++) {
				//add the points of the connected component to the pixelGrid
				cv::Point2i currentPoint = currentBlob.at(j);
				int x = currentPoint.x;
				int y = currentPoint.y;
				splitPixelGrid2[y*width + x] = 200;
			}
			#ifdef DEBUG
			image_output::pixelGridToImage(splitPixelGrid2, width, height, "blobpart2_" + to_string(i) + ".png");
			#endif

			//get the contour from the connected component
			vector<int> contourVector2 = util::getContourVector(splitPixelGrid2, width, height);

			#ifdef DEBUG
			image_output::pixelVectorToImage(contourVector2, width, height, "contour2_" + to_string(i) + ".png");
			#endif
			
			if (contourVector2.size() == 0){
				int dummy = 2;
			}

			//use ronin recursive on the new contour
			vector<vector<double>> resultVector2 = useRoninRecursive(splitPixelGrid2, contourVector2, width, height, qualityValue);
			totalResultVector2.insert(totalResultVector2.end(), resultVector2.begin(), resultVector2.end());
		}

	

	
		//combine the fitted ellipses from the recursive calls and return them
		totalResultVector1.insert(totalResultVector1.end(), totalResultVector2.begin(), totalResultVector2.end());
		return std::move(totalResultVector1);
	}


	//if the fit is good enough:
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
		vector<double> fittedSuperellipse = { xc, yc, theta, a, b, epsilon };
		vector<vector<double>> resultVector;
		resultVector.emplace_back(fittedSuperellipse);
		return std::move(resultVector);
	}
}
