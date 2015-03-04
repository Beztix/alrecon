/**
*************************************************************************
*
* @file as_gpu_input.cpp
*
* Class for preparing the input for GPUAlpha, implementation.
*
************************************************************************/

#include "as_gpu_input.h"

#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "input.h"


using namespace cv;
using namespace std;





void writePixelsToTextFileForAS(string outputFile, int* pixels, int width, int height) {
	int totalNum = 0;

	//calculate number of set pixels
	for (int i = 0; i < height*width; i++) {
		if (pixels[i] != 0){
			totalNum++;
		}
	}

	ofstream myfile;
	myfile.open(outputFile);
	myfile << to_string(totalNum) << endl;

	int num = 0;

	//write the set pixels to file
	for (int i = 0; i < height*width; i++) {
		if (pixels[i] != 0){
			int y = i / width;
			int x = i % width;

			myfile << to_string(num) << " " << to_string(x) << " " << to_string(y) << endl;
			num++;
		}
	}

	myfile.close();

}





int prepareInputForAS(string inputImage, string reducedImgName, string outputFile, int reduction) {
	int width;
	int height;
	
	//loading binary input image
	int* pixels = input::loadPixelsFromImage(inputImage, width, height);
	if (pixels == nullptr){
		return -1;
	}

	//reducing pixels
	int* reducedPixels = input::reducePixels(pixels, width, height, reduction, reducedImgName);

	//write pixels to textfile in Format used by GPUAlpha
	writePixelsToTextFileForAS(outputFile, reducedPixels, width, height);

}