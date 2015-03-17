/**
*************************************************************************
*
* @file input.cpp
*
* Class containing general input methods to load information from images, implementation.
*
************************************************************************/

#include "image_input.h"

#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace std;



namespace image_input {

	/// loads set pixels from a binary image 
	/**
	* This method loads the set pixels from a binary image on disc at the given location/name, returning it as an integer array.
	* The int array represents a grid of width*height.
	* The values contained in the array are the summed-up RGB-values, therefore value != 0 is a set pixel.
	* The method additionally receives two pointers to int values, to store the width and the height of the loaded image.
	*/

	int* loadPixelsFromImage(string imageLocation, int &width, int &height) {
		Mat image;
		image = imread(imageLocation, CV_LOAD_IMAGE_COLOR);

		//Check for invalid input
		if (!image.data)
		{
			cout << "Could not open or find the image" << std::endl;
			return nullptr;
		}

		//get size of the image
		Size size = image.size();
		height = size.height;
		width = size.width;
		
		unsigned char *input = (unsigned char*)(image.data);
		int* pixels = new int[height * width];

		// retrieve the pixel color values
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Vec3b intensity = image.at<Vec3b>(y, x);
				//sum um the rgb values
				pixels[y*width + x] = intensity.val[0] + intensity.val[0] + intensity.val[0];
			}
		}

		return pixels;
	}






	vector<vector<Point>> getContoursFromImage(string imageLocation, int &width, int &height) {
		Mat image;
		image = imread(imageLocation, CV_LOAD_IMAGE_COLOR);

		//Check for invalid input
		if (!image.data)
		{
			cout << "Could not open or find the image" << std::endl;
		}

		//get size of the image
		Size size = image.size();
		height = size.height;
		width = size.width;


		Mat gray;
		cvtColor(image, gray, CV_BGR2GRAY);

		vector<vector<Point>> contours;

		findContours(gray, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));


		//========================
		//TESTING
		cout << "Number of contours: " << contours.size() << endl;

		RNG rng(12345);
		Mat output = Mat(height, width, CV_8UC3);
		//initialize image as black
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				image.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 0);
			}
		}
		//draw the lines given by the input
		for (int i = 0; i < contours.size(); i++) {
			vector<Point> contour = contours.at(i);
			int colorOne = rng.uniform(0, 255);
			int colorTwo = rng.uniform(0, 255);
			int colorThree = rng.uniform(0, 255);
			for (int j = 0; j < contour.size(); j++) {
				Point point = contour.at(j);
				image.at<Vec3b>(point) = Vec3b(colorOne, colorTwo, colorThree);
			}
		}
		imwrite("ContourTest.png", image);
		//========================

		return contours;

	}




}