
#include <string>
#include <iostream>
#include <ceres/ceres.h>

#include "input.h"

using namespace std;
using ceres::Solver;







bool isLeftOf(tuple<double, double> testPoint, tuple<double, double> point1, tuple<double, double> point2) {

	double det = (get<0>(point2) -get<0>(point1))*(get<1>(testPoint) - get<1>(point1)) - (get<0>(testPoint) -get<0>(point1))*(get<1>(point2) -get<1>(point1));
	

	if (det > 0) {
		return true;
	}
	else {
		return false;
	}
}











void splitImage(int* pixelGrid, const int width, const int height, double xc, double yc, double theta, vector<int> &splitPart1, vector<int> &splitPart2) {
		
	const int numPixels = width * height;

	tuple<double, double> point1;
	tuple<double, double> point2;

	//cout << "theta: " << theta << endl;
	//cout << "xc: " << xc << endl;
	//cout << "yc: " << yc << endl;

	//rotate 90 deg
	theta += 1.57079;


	if (theta == 0) {
		point1 = make_tuple(0, yc);
		point2 = make_tuple(width, yc);
	}
	else {
		double tanTheta = tan(theta);
		double t = yc - tanTheta*xc;

		//cout << "tanTheta: " << tanTheta << endl;
		//cout << "t: " << t << endl;

		
		int y1 = int(t);
		point1 = make_tuple(0, y1);
		//cout << "point1: " << 0 << ", " << y1 << endl;

		int y2 = int(tanTheta * width + t);
		point2 = make_tuple(width, y2);
		//cout << "point2: " << width << ", " << y2 << endl;

	}


	double x, y;
	for (int i = 0; i < width*height; i++) {
		if (pixelGrid[i] != 0) {
			y = i / width;
			x = i % width;
			tuple<double, double> testPoint(x, y);
			if (isLeftOf(testPoint, point1, point2)){
				splitPart1.at(i) = 100;
			}
			else {
				splitPart2.at(i) = 100;
			}
		}
	}

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

	int* reducedPixels = input::reducePixels(pixels, width, height, 1000, "test.png");


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

	int* reducedPixels = input::reducePixels(pixels, width, height, 1000, "test.png");


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





std::vector<int> getContoursArray(int* inputPixelGrid, int width, int height) {
	

	int* reducedPixelGrid = input::reducePixels(inputPixelGrid, width, height, 100000, "test.png");

	vector<int> contourVector;

	for (int i = 0; i < width*height; i++) {
		if (reducedPixelGrid[i] != 0) {
			int y = i / width;
			int x = i % width;
			contourVector.emplace_back(x);
			contourVector.emplace_back(y);
		}
	}

	return std::move(contourVector);
}


