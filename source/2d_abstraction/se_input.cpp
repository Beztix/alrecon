#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "input.h"


using namespace cv;
using namespace std;





void writePixelsToTextFile(string outputFile, int* pixels, int width, int height) {
	
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





int prepareInput(string inputImage, string outputFile) {
	int width;
	int height;
	
	//loading binary input image
	int* pixels = input::loadPixelsFromImage(inputImage, &width, &height);
	if (pixels == nullptr){
		return -1;
	}

	//reducing pixels
	int* reducedPixels = input::reducePixels(pixels, width, height, 5);

	writePixelsToTextFile(outputFile, reducedPixels, width, height);

}