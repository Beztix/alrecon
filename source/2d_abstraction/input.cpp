/**
*************************************************************************
*
* @file input.cpp
*
* Class containing general input methods to load information from images, implementation.
*
************************************************************************/

#include "input.h"

#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;



namespace input {

	/// loads set pixels from a binary image 
	/**
	* This method loads the set pixels from a binary image on disc at the given location/name, returning it as an integer array.
	* The values contained in the array are the summed-up RGB-values, therefore value != 0 is a set pixel.
	* The method additionally receives two pointers to int values, to store the width and the height of the loaded image.
	*/

	int* loadPixelsFromImage(string imageLocation, int &width, int &height) {
		Mat image;

		//CAREFUL: Appropriate OpenCV Library (debug or release) must be linked, according to current execution mode
		image = imread(imageLocation, CV_LOAD_IMAGE_COLOR);

		//Check for invalid input
		if (!image.data)
		{
			cout << "Could not open or find the image" << std::endl;
			return nullptr;
		}

		cout << "correctly loaded image in loadPointFromImage()" << std::endl;

		Size size = image.size();
		height = size.height;
		width = size.width;
		//cout << "Height: " << height << endl;
		//cout << "Width: " << width << endl;

		unsigned char *input = (unsigned char*)(image.data);

		//initialize pixel array
		int* pixels = new int[height * width];
		for (int i = 0; i < height*width; i++) {
			pixels[i] = 0;
		}


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








	/// reduces the inner pixels of a binary image
	/**
	* This method takes an array of integers representing pixels of an binary image (value = 0 is not set, value != 0 is set)
	* as well as the width and height of the image corresponding to the pixel array and reduces the "inner" set pixels of the image.
	* Inner pixels are determined using 4-neighborhood and only interior pixels on a grid with given spacing are kept set.
	*
	* The reduced image is written to disc as a png image using opencv.
	*/

	int* reducePixels(int* pixels, int width, int height, int spacing, string reducedImgName) {
		int size = width* height;

		//initialize new pixel array
		int* reducedPixels = new int[size];
		for (int i = 0; i < size; i++) {
			reducedPixels[i] = 0;
		}

		//Reduce the number of set Pixels on the interior
		for (int i = 0; i < size; i++) {

			//current pixel is set
			if (pixels[i] != 0) {
				int y = i / width;
				int x = i % width;

				//current pixel is not on the border of the image
				if (y != 0 && y != height - 1 && x != 0 && x != width - 1) {
					int left = i - 1;
					int right = i + 1;
					int above = (y - 1)*width + x;
					int below = (y + 1)*width + x;

					//current pixel is an interior pixel: all pixels in 4-neighborhood are set too
					if (pixels[left] != 0 && pixels[right] != 0 && pixels[above] != 0 && pixels[below] != 0) {
						//reduce pixels to a grid with spacing 5
						int ymod = y % spacing;
						int xmod = x % spacing;
						if (ymod == 0 && xmod == 0) {
							reducedPixels[i] = 1;
						}
						else {
							reducedPixels[i] = 0;
						}
					}

					//current pixel is not an interior pixel: not all pixels in 4-neighborhood are set
					else {
						reducedPixels[i] = 1;
					}
				}
			}
		}

		//// initializing reduced image
		//Mat reducedImage = Mat(height, width, CV_8UC3);
		//for (int y = 0; y < height; y++) {
		//	for (int x = 0; x < width; x++) {
		//		reducedImage.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 0);
		//	}
		//}

		//// Write reduced pixels to color value array
		//for (int i = 0; i < height*width; i++) {
		//	if (reducedPixels[i] != 0){
		//		int y = i / width;
		//		int x = i % width;
		//		reducedImage.at<Vec3b>(Point(x, y)) = Vec3b(250, 250, 250);
		//	}
		//}

		//cout << "Writing reduced image to disc: " << reducedImgName << endl;
		//imwrite(reducedImgName, reducedImage);

		return reducedPixels;
	}









void renderImage(string name, int width, int height, int num, double data[1000]) {


	Mat image = Mat(height, width, CV_8UC3);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			image.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 0);
		}
	}


	for (int i = 0; i < num; i++) {
		Point point1 = Point(data[4 * i], data[4 * i + 1]);
		Point point2 = Point(data[4 * i + 2], data[4 * i + 3]);
		line(image, point1, point2, Scalar(255, 200, 20), 1, 8, 0);
	}

	imwrite(name, image);


}

}