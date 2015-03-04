/**
*************************************************************************
*
* @file se_input.cpp
*
* Input class for preparing the input for superellipse calculation, implementation.
*
************************************************************************/

#include "se_input.h"

#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "input.h"


using namespace cv;
using namespace std;





void writePixelsToTextFileForRonin(string outputFile, int* pixels, int width, int height) {
	
	ofstream myfile;
	myfile.open(outputFile);
	myfile << "pixel" << endl;
	myfile << "list: 0" << endl;



	//write the set pixels to file
	for (int i = 0; i < height*width; i++) {
		if (pixels[i] != 0){
			int y = i / width;
			int x = i % width;

			myfile << to_string(x) << " " << to_string(y) << endl;
		}
	}
	myfile << "-1 -1" << endl;

	myfile.close();

}



void writePixelsToTextFileForCeres(string outputFile, int* pixels, int width, int height) {

	ofstream myfile;
	myfile.open(outputFile);
	myfile << "w " << width << endl;
	myfile << "h " << height << endl;

	//write the set pixels to file
	for (int i = 0; i < height*width; i++) {
		if (pixels[i] != 0){
			int y = i / width;
			int x = i % width;

			myfile << to_string(x) << " " << to_string(y) << endl;
		}
	}
	myfile.close();

}




int prepareInputForRonin(string inputImage, string reducedImgName, string pixelFile, string contourFile, int reduction) {
	int width;
	int height;
	
	//loading binary input image
	int* pixels = input::loadPixelsFromImage(inputImage, &width, &height);
	if (pixels == nullptr){
		return -1;
	}

	writePixelsToTextFileForCeres(pixelFile, pixels, width, height);

	//reducing pixels
	int* reducedPixels = input::reducePixels(pixels, width, height, reduction, reducedImgName);

	writePixelsToTextFileForRonin(contourFile, reducedPixels, width, height);
	return 0;
}



int prepareInputForCeres(string inputImage, string reducedImgName, string outputFile, int reduction) {
	int width;
	int height;

	//loading binary input image
	int* pixels = input::loadPixelsFromImage(inputImage, &width, &height);
	if (pixels == nullptr){
		return -1;
	}

	//reducing pixels
	int* reducedPixels = input::reducePixels(pixels, width, height, reduction, reducedImgName);

	writePixelsToTextFileForCeres(outputFile, reducedPixels, width, height);
	writePixelsToTextFileForCeres("allPixels", pixels, width, height);
	return 0;
}





int pixFileToImage(string input, string output) {

	//===============
	//read once to determine width and height of image
	//===============

	
	ifstream infile(input);

	string line;
	int i;

	// reading the first two lines (not needed here)
	infile >> line;
	//cout << line << endl;
	infile >> line >> i;
	//cout << line << i << endl;

	int x;
	int y;
	int width = 0;
	int height = 0;

	// reading pixels from textfile
	while (infile >> x >> y) {
		//cout << x << " " << y << endl;
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


	ifstream infileTwo(input);

	// reading the first two lines (not needed here)
	infileTwo >> line;
	cout << line << endl;
	infileTwo >> line >> i;
	cout << line << i << endl;


	Point point;

	// reading pixels from textfile
	while (infileTwo >> x >> y) {
		if (x != -1) {
			point = Point(x, y);
			image.at<Vec3b>(point) = Vec3b(255, 255, 255);
		}
		else{	
			//x == -1 - new contour or end of file
			if (!(infileTwo >> line >> i)) {	
				//next line not "line: i" : end of file
				break;
			}
			
		}
	}




	imwrite(output, image);

	return 0;

}




int pixelFileToImage(string input, string output) {

	
	ifstream infile(input);



	int x;
	int y;
	int width = 0;
	int height = 0;
	string line;

	infile >> line >> width;
	infile >> line >> height;

	Mat image = Mat(height, width, CV_8UC3);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			image.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 0);
		}
	}

	Point point;

	// reading pixels from textfile
	while (infile >> x >> y) {
		point = Point(x, y);
		image.at<Vec3b>(point) = Vec3b(255, 255, 255);
		
	}


	imwrite(output, image);

	return 0;

}








