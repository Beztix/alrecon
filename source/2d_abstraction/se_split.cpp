
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











void splitImage(string inputFile, double xc, double yc, double theta, string outputFile1, string outputFile2) {
		
	ifstream infile(inputFile);
	ofstream outfile1(outputFile1);
	ofstream outfile2(outputFile2);


	int width;
	int height;
	string dummy;
	infile >> dummy >> width;
	infile >> dummy >> height;

	outfile1 << "w " << width << endl;
	outfile1 << "h " << height << endl;

	outfile2 << "w " << width << endl;
	outfile2 << "h " << height << endl;


	tuple<double, double> point1;
	tuple<double, double> point2;

	cout << "theta: " << theta << endl;
	cout << "xc: " << xc << endl;
	cout << "yc: " << yc << endl;

	//rotate 90 deg
	theta += 1.57079;


	if (theta == 0) {
		point1 = make_tuple(0, yc);
		point2 = make_tuple(width, yc);
	}
	else {
		double tanTheta = tan(theta);
		double t = yc - tanTheta*xc;

		cout << "tanTheta: " << tanTheta << endl;
		cout << "t: " << t << endl;

		
		int y1 = int(t);
		point1 = make_tuple(0, y1);
		cout << "point1: " << 0 << ", " << y1 << endl;

		int y2 = int(tanTheta * width + t);
		point2 = make_tuple(width, y2);
		cout << "point2: " << width << ", " << y2 << endl;

	}

	double x, y;
	while (infile >> x >> y) {
		tuple<double, double> testPoint (x, y);
		if (isLeftOf(testPoint, point1, point2)){
			outfile1 << x << " " << y << endl;
		}
		else {
			outfile2 << x << " " << y << endl;
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