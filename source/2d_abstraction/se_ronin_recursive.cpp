/**
*************************************************************************
*
* @file se_ronin_recursive.cpp
*
* 
*
************************************************************************/

#include <vector>

#include "se_ronin_core.h"
#include "se_split.h"
#include "util.h"

using namespace std;




/// evaluates the quality of a superellipsefit by returning a fitValue, the lower the better
/**
* This method uses the contour and the parameters of the superellipse fitted to that contour to calculate a quality of the fit (fitValue).
* The fitValue is calculated as the sum of the distances of each point of the contour to the superellipse
*/

double evaluateSuperellipseFit(vector<int> contourVector, double xc, double yc, double theta, double a, double b, double epsilon) {
	int no_contourPixels = int(contourVector.size()) / 2;
	int* contourList = &contourVector[0];
	double cosine2 = cos(-theta);
	double sine2 = sin(-theta);
	double tx, ty;
	double x, y, x2, y2, value;
	double totalValue = 0;

	double exponent = 2.0 / epsilon;


	for (int i = 0; i < no_contourPixels; i++){
		x = contourList[i * 2];
		y = contourList[i * 2 + 1];
		tx = x - xc;
		ty = y - yc;
		x2 = (tx * cosine2 - ty * sine2);
		y2 = (tx * sine2 + ty * cosine2);

		double firstTerm = (fabs(x2) / a);
		double secondTerm = (fabs(y2) / b);
		value = fabs(pow(firstTerm, exponent) + pow(secondTerm, exponent) - 1.0);
		totalValue += value;
	}

	//cout << "Total Value: " << totalValue << endl;
	return totalValue;

}








/// uses the ronin fitting of superellipses in a recursive way to fit mutliple superellipses to a contour until a certain quality level is reached
/**
* This method is called to recursively fit superellipses to a given contour, returning the fitted superellipse if the fit is of high quality
* and splitting the input to fit two separate superellipses if the fit by one superellipse is of low quality.
*/

vector<vector<double>> useRoninRecursive(int* pixelGrid, vector<int> contourVector, const int width, const int height) {

	//fit one ellipse to the contout in the contourVector
	int no_contourPixels = int(contourVector.size()) / 2;
	int* contourList = &contourVector[0];
	double xc, yc, theta, a, b, epsilon;
	fitEllipseRonin2(contourList, no_contourPixels, &xc, &yc, &theta, &a, &b, &epsilon);


	//evaluate the fit
	double currentValue = evaluateSuperellipseFit(contourVector, xc, yc, theta, a, b, epsilon);

	//if the fit is not good enough yet:
	if (currentValue > 100) {

		//split the full pixel grid
		vector<int> splitPixelPart1(width*height);
		vector<int> splitPixelPart2(width*height);
		splitImage(pixelGrid, width, height, xc, yc, theta, splitPixelPart1, splitPixelPart2);
		int* splitPixelGrid1 = &splitPixelPart1[0];
		int* splitPixelGrid2 = &splitPixelPart2[0];

		//get the two new contours from the splitted image
		vector<int> contourVector1 = util::getContoursArray(splitPixelGrid1, width, height);
		vector<int> contourVector2 = util::getContoursArray(splitPixelGrid2, width, height);

		//use ronin recursive on the two new contours
		vector<vector<double>> resultVector1 = useRoninRecursive(splitPixelGrid1, contourVector1, width, height);
		vector<vector<double>> resultVector2 = useRoninRecursive(splitPixelGrid2, contourVector2, width, height);

		//combine the fitted ellipses from the recursive calls and return them
		resultVector1.insert(resultVector1.end(), resultVector2.begin(), resultVector2.end());
		return resultVector1;
	}

	//if the fit is good enough:
	else {
		//return the fitted ellipse
		vector<double> fittedSuperellipse = { xc, yc, theta, a, b, epsilon };
		vector<vector<double>> resultVector;
		resultVector.emplace_back(fittedSuperellipse);
		return resultVector;
	}
}
