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

		/*image_output::pixelVectorToImage(contourVector, width, height, "deb_contourVector.png");
		image_output::pixelGridToImage(inputPixelGrid, width, height, "deb_inputPixelGrid.png");
*/


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











	vector<cv::Point> bresenhamLine(const cv::Point p1, const cv::Point p2) {
		double x1 = p1.x;
		double y1 = p1.y;
		double x2 = p2.x;
		double y2 = p2.y;

		vector<cv::Point> line;

		const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
		if (steep)
		{
			std::swap(x1, y1);
			std::swap(x2, y2);
		}

		if (x1 > x2)
		{
			std::swap(x1, x2);
			std::swap(y1, y2);
		}

		const double dx = x2 - x1;
		const double dy = fabs(y2 - y1);

		double error = dx / 2.0f;
		const int ystep = (y1 < y2) ? 1 : -1;
		int y = (int)y1;

		const int maxX = (int)x2;

		for (int x = (int)x1; x<=maxX; x++)
		{
			if (steep)
			{
				line.push_back(Point(y, x));
			}
			else
			{
				line.push_back(Point(x, y));
			}

			error -= dy;
			if (error < 0)
			{
				y += ystep;
				error += dx;
			}
		}
		return line;
	}







	int testSegmentsIntersection(cv::Point a1, cv::Point a2, cv::Point b1, cv::Point b2) {

		double den = ((b2.y - b1.y)*(a2.x - a1.x) - (b2.x - b1.x)*(a2.y - a1.y));
		double num1 = ((b2.x - b1.x)*(a1.y - b1.y) - (b2.y - b1.y)*(a1.x - b1.x));
		double num2 = ((a2.x - a1.x)*(a1.y - b1.y) - (a2.y - a1.y)*(a1.x - b1.x));
		double u1 = num1 / den;
		double u2 = num2 / den;
		
		// the lines are coincidents
		if (den == 0 && num1 == 0 && num2 == 0) {
			return 10;
		}
		
		//the two segments are parallel
		if (den == 0) {
			return 11;
		}

		//the two segments do not collide
		if (u1 < 0 || u1 > 1 || u2 < 0 || u2 > 1) {
			return 12;
		}
		
		//the two segments do collide
		return 13;
	}



	/**
	* This method intersects two lines, if it is known they have an intersection!
	* If they don't have an intersection the Point (0,0) is returned, but this is not intenden to be a valid test if the liens do intersect.
	*
	*/

	cv::Point2d intersectLines(cv::Point a1, cv::Point a2, cv::Point b1, cv::Point b2) {
		cv::Point2d x = b1 - a1;
		cv::Point2d da = a2 - a1;
		cv::Point2d db = b2 - b1;

		double cross = da.x*db.y - da.y*db.x;

		if (abs(cross) < 1e-8) {
			cv::Point2d r = cv::Point2d(0.0, 0.0);
			return r;
		}
		double t = (x.x * db.y - x.y * db.x) / cross;
		cv::Point2d a1d = cv::Point2d(a1.x, a1.y);
		cv::Point2d r = a1d + da*t;
		return r;
	}












	void getContours(string inputFile, string contourFile) {
		ifstream infile(inputFile);
		int width, height;
		string dummy;
		infile >> dummy >> width;
		infile >> dummy >> height;


		int* pixels = new int[height * width];
		for (int i = 0; i < height*width; i++) {
			pixels[i] = 0;
		}

		int x, y;

		while (infile >> x >> y) {
			pixels[x + y*width] = 100;
		}

		vector<int> reducedPixels = util::reducePixels(pixels, width, height, 1000, "test.png");


		ofstream outfile(contourFile);

		outfile << "w " << width << endl;
		outfile << "h " << height << endl;

		for (int i = 0; i < width*height; i++) {
			if (reducedPixels[i] != 0) {
				int y = i / width;
				int x = i % width;
				outfile << x << " " << y << endl;
			}
		}

	}





	void getContoursRonin(string inputFile, string contourFile) {
		ifstream infile(inputFile);
		int width, height;
		string dummy;
		infile >> dummy >> width;
		infile >> dummy >> height;


		int* pixels = new int[height * width];
		for (int i = 0; i < height*width; i++) {
			pixels[i] = 0;
		}

		int x, y;

		while (infile >> x >> y) {
			pixels[x + y*width] = 100;
		}

		vector<int> reducedPixels = util::reducePixels(pixels, width, height, 1000, "test.png");


		ofstream outfile(contourFile);

		outfile << "pixel" << endl;
		outfile << "list: 0" << endl;

		for (int i = 0; i < width*height; i++) {
			if (reducedPixels[i] != 0) {
				int y = i / width;
				int x = i % width;
				outfile << x << " " << y << endl;
			}
		}
		outfile << -1 << " " << -1 << endl;

	}










}





