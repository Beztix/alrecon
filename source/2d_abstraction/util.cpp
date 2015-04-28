/**
*************************************************************************
*
* @file util.cpp
*
* Class containing all utility methods which currently have no other class they would belong to, implementation.
*
************************************************************************/

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "image_output.h"

using namespace std;
using namespace cv;

namespace util {




	/// sorts pixels from a text file lexicographically 
	/**
	* This method loads the pixels from a text file on disc at the given location/name, given the pixels in the form "x y\n"
	* (one pixel per line, separated by whitespace).
	* The pixels are sorted lexicographically using the std algorithm sort() and are then written to the text file at given
	* location/name in the same format as the input.
	*/

	void sortPixelFile(string input, string output) {

		//read points from input file
		ifstream infile(input);
		vector<tuple<int, int>> points;
		int x;
		int y;
		while (infile >> x >> y) {
			tuple<int, int> point(x, y);
			points.emplace_back(point);
		}

		//sort the points lexicographically using the std algorithm sort()
		sort(points.begin(), points.end());

		//write sorted points to output file
		ofstream myfile;
		myfile.open(output);
		for (int i = 0; i < points.size(); i++) {
			tuple<int, int> point = points.at(i);
			myfile << to_string(get<0>(point)) << " " << to_string(get<1>(point)) << endl;
		}
	}













	/// reduces the inner pixels of a binary image
	/**
	* This method takes an array of integers representing pixels of an binary image (value = 0 is not set, value != 0 is set)
	* as well as the width and height of the image corresponding to the pixel array and reduces the "inner" set pixels of the image.
	* Inner pixels are determined using 4-neighborhood and only interior pixels on a grid with given spacing are kept set.
	*
	* The reduced image is written to disc as a png image using opencv.
	*/

	vector<int> reducePixels(int* pixels, int width, int height, int spacing, string reducedImgName) {
		int size = width* height;

		//initialize new pixel array
		vector<int> reducedPixels;
		reducedPixels.resize(size);
		std::fill(reducedPixels.begin(), reducedPixels.end(), 0);

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
						//reduce pixels to a grid with given spacing
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
		return reducedPixels;
	}







	void getContourPixels(vector<int> inputPixelGrid, vector<int> &contourPixelGrid, int width, int height) {
		int size = width* height;

		std::fill(contourPixelGrid.begin(), contourPixelGrid.end(), 0);

		int target_index = 0;
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				target_index = y*width + x;
				//current pixel is set
				if (inputPixelGrid[target_index] != 0) {
					
					//current pixel is not on the border of the image
					if (y != 0 && y != height - 1 && x != 0 && x != width - 1) {
						int left = target_index - 1;
						int right = target_index + 1;
						int above = (y - 1)*width + x;
						int below = (y + 1)*width + x;

						//current pixel is a contour pixel (one or more pixels in the 4-neighborhood are not set)
						if (inputPixelGrid[left] == 0 || inputPixelGrid[right] == 0 || inputPixelGrid[above] == 0 || inputPixelGrid[below] == 0) {
							//-> set this pixel to 200
							contourPixelGrid[target_index] = 200;
						}

						////current pixel is not an interior pixel: not all pixels in 4-neighborhood are set
						//else {
						//	contourPixelGrid[target_index] = 200;
						//}
					}
				}

			}
		}
		
	}








	std::vector<int> getContourVector(vector<int> inputPixelGrid, int width, int height) {
		vector<int> contourVector;

		int target_index = 0;
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				target_index = y*width + x;

				//current pixel is set
				if (inputPixelGrid[target_index] != 0) {
					
					//current pixel is not on the border of the image
					if (y != 0 && y != height - 1 && x != 0 && x != width - 1) {
						//calculate 4-neighborhood pixels
						int left = target_index - 1;
						int right = target_index + 1;
						int above = (y - 1)*width + x;
						int below = (y + 1)*width + x;

						//current pixel is a contour pixel (one or more pixels in the 4-neighborhood are not set)
						if (inputPixelGrid[left] == 0 || inputPixelGrid[right] == 0 || inputPixelGrid[above] == 0 || inputPixelGrid[below] == 0) {
							contourVector.push_back(x);
							contourVector.push_back(y);
						}
					}
				}

			//end loops
			}
		}

		image_output::pixelVectorToImage(contourVector, width, height, "deb_contourVector.png");
		image_output::pixelGridToImage(inputPixelGrid, width, height, "deb_inputPixelGrid.png");



		return std::move(contourVector);
	}




	int* getFilledPixelGridFromContour(vector<cv::Point> contourPoints, int width, int height) {
		//create an empty image
		Mat image = Mat(height, width, CV_8UC3);

		image.setTo(Scalar(0, 0, 0));

		
		//wrap the contour in a contours vector
		vector<vector<cv::Point>> contours;
		contours.push_back(contourPoints);

		//draw the filled contour to the image
		Scalar color(255, 255, 255);
		drawContours(image, contours, -1, color, CV_FILLED, 8);


		//imwrite("test.png", image);



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


}

