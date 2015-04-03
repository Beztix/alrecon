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
		if (isinf(value)) {
			double temp = 5.0;
		}
		
		
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

vector<vector<double>> useRoninRecursive(int* pixelGrid, vector<int> contourVector, const int width, const int height, const int qualityValue) {

	//fit one ellipse to the contout in the contourVector
	int no_contourPixels = int(contourVector.size()) / 2;
	int* contourList = &contourVector[0];

	image_output::pixelVectorToImage(contourVector, width, height, "contour.png");

	double xc, yc, theta, a, b, epsilon;
	fitEllipseRonin2(contourList, no_contourPixels, &xc, &yc, &theta, &a, &b, &epsilon);

	if (a < b) {
		double temp = a;
		a = b;
		b = temp;
		theta += 1.57079;
	}

	image_output::renderSuperEllipseToImage("ellipse.png", width, height, xc, yc, theta, a, b, epsilon);

	//evaluate the fit
	double currentValue = evaluateSuperellipseFit(contourVector, xc, yc, theta, a, b, epsilon);

	//if the fit is not good enough yet:
	if (currentValue > qualityValue) {

		//split the full pixel grid
		/*vector<int> splitPixelPart1(width*height);
		vector<int> splitPixelPart2(width*height);
		splitImageToGrids(pixelGrid, width, height, xc, yc, theta, splitPixelPart1, splitPixelPart2);
		int* splitPixelGrid1 = &splitPixelPart1[0];
		int* splitPixelGrid2 = &splitPixelPart2[0];*/



		//split the pixel grid
		cv::Mat part1;
		cv::Mat part2;
		splitImageToMats(pixelGrid, width, height, xc, yc, theta, part1, part2);
		imwrite("part1.png", part1);
		imwrite("part2.png", part2);


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

			//create a pixelGrid from the blob points
			int* splitPixelGrid1 = new int[width * height];
			fill_n(splitPixelGrid1, width*height, 0);
			for (int j = 0; j < currentBlob.size(); j++) {
				//add the points of the connected component to the pixelGrid
				cv::Point2i currentPoint = currentBlob.at(j);
				int x = currentPoint.x;
				int y = currentPoint.y;
				splitPixelGrid1[y*width + x] = 200;
			}

			image_output::pixelGridToImage(splitPixelGrid1, width, height, "blobpart1.png");


			//get the contour from the connected component
			vector<int> contourVector1 = util::getContoursArray(splitPixelGrid1, width, height);

			image_output::pixelVectorToImage(contourVector1, width, height, "contour1.png");

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

			//create a pixelGrid from the blob points
			int* splitPixelGrid2 = new int[width * height];
			fill_n(splitPixelGrid2, width*height, 0);
			for (int j = 0; j < currentBlob.size(); j++) {
				//add the points of the connected component to the pixelGrid
				cv::Point2i currentPoint = currentBlob.at(j);
				int x = currentPoint.x;
				int y = currentPoint.y;
				splitPixelGrid2[y*width + x] = 200;
			}

			//get the contour from the connected component
			vector<int> contourVector2 = util::getContoursArray(splitPixelGrid2, width, height);

			image_output::pixelVectorToImage(contourVector2, width, height, "contour2.png");

			//use ronin recursive on the new contour
			vector<vector<double>> resultVector2 = useRoninRecursive(splitPixelGrid2, contourVector2, width, height, qualityValue);
			totalResultVector2.insert(totalResultVector2.end(), resultVector2.begin(), resultVector2.end());
		}

	

	
		//combine the fitted ellipses from the recursive calls and return them
		totalResultVector1.insert(totalResultVector1.end(), totalResultVector2.begin(), totalResultVector2.end());
		return totalResultVector1;
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
