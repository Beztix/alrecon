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
		////initialize pixel array
		//for (int i = 0; i < height*width; i++) {
		//	pixels[i] = 0;
		//}

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





}