/**
*************************************************************************
*
* @file main.cpp
*
* Main class for executing the various algorithms of alrecon.
*
************************************************************************/

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>


// CGAL stuff
#include "as_cgal.h"



// GPUAlpha stuff
#include "as_gpu_input.h"


// Superellipses stuff
#include "se_rendering.h"
#include "se_input.h"
//#include "se_epsilon_new.h"
#include "se_epsilon2_new.h"
#include "ellipseFit.h"





using namespace std;




void sortPixelFile(string input, string output) {



	ifstream infile(input);
	vector<tuple<int, int>> points;
	int x;
	int y;
	while (infile >> x >> y) {
		tuple<int, int> point(x, y);
		points.emplace_back(point);
	}

	sort(points.begin(), points.end());



	ofstream myfile;
	myfile.open(output);

	for (int i = 0; i < points.size(); i++) {
		tuple<int, int> point = points.at(i);
		myfile << to_string(get<0>(point)) << " " << to_string(get<1>(point)) << endl;
	}


}















int main() {



	bool USE_CGAL =					false;
	bool USE_GPUALPHA =				false;
	bool USE_SUPERELLIPSES_RONIN =	false;
	bool USE_SUPERELLIPSES_CERES =	true;


	


	//==========================
	//====       CGAL       ====
	//==========================
	if (USE_CGAL) {

		//++++++++++++++++++++++++++++
		string inputName =	"image";
		int reduction =		5;
		//++++++++++++++++++++++++++++

		string inputImage = "images/" + inputName + ".png";
		string reducedImgName = "as_cgal_" + inputName + "_reducedImage.png";
		string outputImage = "as_cgal_" + inputName + "_alphaImage.png";





		useCgal(inputImage, reducedImgName, outputImage, reduction);
	}








	//==========================
	//====     GPUAlpha     ====
	//==========================
	if (USE_GPUALPHA) {

		//++++++++++++++++++++++++++++
		bool PREPARE = true;

		string inputName =	"1920x1080_high";
		int reduction =		5; 
		//++++++++++++++++++++++++++++


		string inputImage = "images/" + inputName + ".png";
		string reducedImgName = "as_gpu_" + inputName + "_reducedImage.png";
		string pixelFile = "as_gpu_" + inputName + "_pixels.txt";


		if (PREPARE) {
			cout << "Preparing input for GPUAlpha" << endl;
			int err = prepareInputForAS(inputImage, reducedImgName, pixelFile, reduction);
			cout << "Prepared input from file " << inputImage << " to file " << pixelFile << endl;
		}

	}








	//===========================
	//====   Superellipses   ====
	//===========================
	if (USE_SUPERELLIPSES_RONIN) {

		//++++++++++++++++++++++++++++
		bool RESTORE_IMAGE = false;
		bool PREPARE =	false;
		bool COMPUTE =	true;
		bool RENDER =	true;

		string inputName =	"mouse_part_sort";
		int reduction =		1;
		//++++++++++++++++++++++++++++

		string inputImage = "images/" + inputName + ".png";
		string reducedImgName = "se_" + inputName + "_reducedImage.png";
		//string pixelFile = "se_" + inputName + "_pixels.txt";
		string pixelFile = inputName + ".txt";
		string ellipseTextFile = "se_" + inputName + "_textellipses";
		string ellipseImage = "se_" + inputName + "_ellipseImage.png";
		string restorePixFile = "mouse_part_sort.txt";
		string restoredImage = "mouse_part_sort.png";


		if (RESTORE_IMAGE) {
			int err = pixFileToImage(restorePixFile, restoredImage);

		}


	//	sortPixelFile("mousePart_restoredPixels.txt", "mousePart_restoredPixelsSorted.txt");


		if (PREPARE) {
			cout << "Preparing input for superellipses - Ronin" << endl;
			int err = prepareInputForRonin(inputImage, reducedImgName, pixelFile, reduction);
			cout << "Prepared input from file " << inputImage << " to file " << pixelFile << endl;
		}


		if (COMPUTE) {
			cout << "Calculating superellipses" << endl;
			char *pixelFileChar = new char[pixelFile.size() + 1];
			pixelFileChar[pixelFile.size()] = 0;
			memcpy(pixelFileChar, pixelFile.c_str(), pixelFile.size());

			char *ellipseTextFileChar = new char[ellipseTextFile.size() + 1];
			ellipseTextFileChar[ellipseTextFile.size()] = 0;
			memcpy(ellipseTextFileChar, ellipseTextFile.c_str(), ellipseTextFile.size());

			calculateEllipse2(pixelFileChar, ellipseTextFileChar);
			cout << "Calculated ellipses for file " << pixelFile << " to file " << ellipseTextFile << endl;
		}


		if (RENDER) {
			cout << "Rendering superellipses" << endl;
			int err = processSuperellipsesFromTextfile(ellipseTextFile, ellipseImage);
			cout << "Rendered ellipses from file " << ellipseTextFile << " to Image " << ellipseImage << endl;
		}

	}





	if (USE_SUPERELLIPSES_CERES) {

		//++++++++++++++++++++++++++++
		bool PREPARE =			true;
		bool COMPUTE =			true;
		bool RENDER =			true;

		string inputName = "ellipsetest7";
		int reduction = 100;
		//++++++++++++++++++++++++++++

		string inputImage = "images/" + inputName + ".png";
		string reducedImgName = "se_" + inputName + "_reducedImage.png";
		string pixelFile = "se_" + inputName + "_pixels.txt";
		string ellipseTextFile = "se_" + inputName + "_textellipses";
		string ellipseImage = "se_" + inputName + "_ellipseImage.png";
		string restorePixFile = "mouse_part_sort.txt";
		string restoredImage = "mouse_part_sort.png";



		if (PREPARE) {
			cout << "Preparing input for superellipses - Ceres" << endl;
			int err = prepareInputForCeres(inputImage, reducedImgName, pixelFile, reduction);
			cout << "Prepared input from file " << inputImage << " to file " << pixelFile << endl;
		}


		if (COMPUTE) {
			cout << "Calculating superellipses - Ceres" << endl;
			int err = useCeres(pixelFile, ellipseTextFile);
			cout << "Calculated ellipses for file " << pixelFile << " to file " << ellipseTextFile << endl;
		}


		if (RENDER) {
			cout << "Rendering superellipses" << endl;
			int err = processSuperellipsesFromTextfileCeres(ellipseTextFile, ellipseImage);
			cout << "Rendered ellipses from file " << ellipseTextFile << " to Image " << ellipseImage << endl;
		}




	}



}