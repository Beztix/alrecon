#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;


int processSuperellipsesFromTextfile(string input, string output) {

	//===============
	//read once to determine width and height of image
	//===============

	ifstream infile(input);

	string line;
	int i;

	// reading the first two lines (not needed here)
	infile >> line;
	cout << line << endl;
	infile >> line >> i;
	cout << line << i << endl;

	int x;
	int y;
	int width= 0;
	int height = 0;

	// reading superellipse parameters from textfile
	while (infile >> x >> y) {
		cout << x << " " << y << endl;
		if (x > width) {
			width = x;
		}
		if (y > height) {
			height = y;
		}
	}

	width += 10;
	height += 10;


	Mat image = Mat(height, width, CV_8UC3);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			image.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 0);
		}
	}


	//===============
	//read again to insert set pixels into image
	//===============


	ifstream infile(input);

	string line;
	int i;

	// reading the first two lines (not needed here)
	infile >> line;
	cout << line << endl;
	infile >> line >> i;
	cout << line << i << endl;

	int x;
	int y;
	int width = 0;
	int height = 0;

	Point point;

	// reading superellipse parameters from textfile
	while (infile >> x >> y) {
		if (x != -1) {
			point = Point(x, y);
			image.at<Vec3b>(point) = Vec3b(255, 255, 255);
		}
	}
	
	imwrite(output, image);

	return 0;

}






