/**
*************************************************************************
*
* @file image_output.cpp
*
* 
*
************************************************************************/

#include "image_output.h"

#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "se_rendering.h"

using namespace cv;
using namespace std;

namespace image_output {



	/// renders lines given by an array to an image with given name
	/**
	* 
	*/

	void renderArrayOfLinesToImage(string name, int width, int height, int num, int data[1000]) {

		//create an image with given width and height
		Mat image = Mat(height, width, CV_8UC3);

		//initialize image as black
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				image.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 0);
			}
		}

		//draw the lines given by the input
		for (int i = 0; i < num; i++) {
			Point point1 = Point(data[4 * i], data[4 * i + 1]);
			Point point2 = Point(data[4 * i + 2], data[4 * i + 3]);
			line(image, point1, point2, Scalar(255, 200, 20), 1, 8, 0);
		}

		imwrite(name, image);
	}






	/// renders a vector of pixels as white dots on black background
	/**
	*
	*/

	void pixelVectorToImage(vector<int> pixels, int width, int height, string imageName) {
		Mat image = Mat(height, width, CV_8UC3);
		image.setTo(cv::Scalar(0, 0, 0));
		int x, y;
		Point point;
		for (int i = 0; i < int(pixels.size() / 2); i++) {
			x = pixels.at(2 * i);
			y = pixels.at(2 * i + 1);
			point = Point(x, y);
			image.at<Vec3b>(point) = Vec3b(255, 255, 255);
		}

		imwrite(imageName, image);
	}






	void pixelGridToImage(vector<int> pixelGrid, int width, int height, string imageName) {
		Mat image = Mat(height, width, CV_8UC3);
		image.setTo(cv::Scalar(0, 0, 0));

		Point point;
		int y;
		int x;
		for (int i = 0; i < width*height; i++) {
			if (pixelGrid[i] != 0) {
				y = i / width;
				x = i % width;
				point = Point(x, y);
				image.at<Vec3b>(point) = Vec3b(255, 255, 255);
			}
		}

		imwrite(imageName, image);
	}







	/// renders a vector of pixels representing superellipses as blue ellipses on black background
	/**
	*
	*/

	void writeSuperellipsesToImage(vector<vector<tuple<double, double>>> pointListContainer, string imgName, int width, int height) {

		//initialize image
		Mat image = Mat(height, width, CV_8UC3);
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				image.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 0);
			}
		}

		//go through all superellipses
		for (vector<vector<tuple<double, double>>>::iterator iterator = pointListContainer.begin(); iterator != pointListContainer.end(); iterator++) {

			//get current superellipse
			vector<tuple<double, double>> pointList = *iterator;

			//go through all rendered points
			Point previousPoint, firstPoint, currentPoint;
			bool first = true;
			for (vector<tuple<double, double>>::iterator it = pointList.begin(); it != pointList.end(); it++) {
				tuple<double, double> point = *it;
				double x = get<0>(point);
				double y = get<1>(point);
				x += 0.5;		//rounding instead of cutting off by casting to int
				y += 0.5;
				int xInt = static_cast<int>(x);
				int yInt = static_cast<int>(y);
				currentPoint = Point(xInt, yInt);

				//draw line from previous point to current point
				if (!first) {
					line(image, previousPoint, currentPoint, Scalar(255, 200, 20), 1, 8, 0);
				}
				else {
					firstPoint = currentPoint;
				}

				previousPoint = currentPoint;
				first = false;
			}
			//draw line from last point to first point
			line(image, currentPoint, firstPoint, Scalar(255, 200, 20), 1, 8, 0);

		}

		imwrite(imgName, image);
	}




	void renderSuperEllipseToImage(string imgName, int imgWidth, int imgHeight, double xc, double yc, double theta, double a, double b, double epsilon) {
		vector<double> superellipse = { xc, yc, theta, a, b, epsilon };
		vector<vector<double>> ellipsesVector;
		ellipsesVector.emplace_back(superellipse);

		processSuperellipsesFromVector(ellipsesVector, imgName, imgWidth, imgHeight);
	
	}






	void combineOriginalAndEllipseImage(string originalImageLocation, string ellipseImageLocation, string outputImageLocation) {

		Mat originalImage;
		originalImage = imread(originalImageLocation, CV_LOAD_IMAGE_COLOR);

		//get size of the image
		Size size = originalImage.size();
		int height = size.height;
		int width = size.width;

		Mat ellipseImage;
		ellipseImage = imread(ellipseImageLocation, CV_LOAD_IMAGE_COLOR);



		Mat outputImage = originalImage;

		// retrieve the pixel color values
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Point point = Point(x, y);
				Vec3b intensity = ellipseImage.at<Vec3b>(point);
				//sum um the rgb values
				int colorValue = intensity.val[0] + intensity.val[0] + intensity.val[0];

				//add ellipses to outputImage
				if (colorValue != 0) {
					outputImage.at<Vec3b>(point) = intensity;
				}
			}
		}


		imwrite(outputImageLocation, outputImage);

	}







}


