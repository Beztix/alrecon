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
#include <chrono>

#include "image_input.h"
#include "image_output.h"
#include "util.h"


// CGAL stuff
#include "as_cgal.h"



// GPUAlpha stuff
#include "as_gpu_input.h"


// Superellipses stuff
#include "se_rendering.h"
#include "se_ronin_core.h"
#include "se_ronin_recursive.h"
#include <ceres/ceres.h>
#include "se_split.h"


#include "convexHull.h"
#include "convexHullForC.h"

using namespace std;





int main() {



	bool USE_CGAL =					false;
	bool USE_GPUALPHA =				false;
	bool USE_SUPERELLIPSES_RONIN =	true;
	bool USE_SUPERELLIPSES_CERES =	false;







	//############################################
	//##########          CGAL          ##########       
	//############################################

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









	//############################################
	//##########        GPUAlpha        ##########       
	//############################################

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









	//#######################################################
	//##########        Superellipses Ronin        ##########       
	//#######################################################

	if (USE_SUPERELLIPSES_RONIN) {

		//++++++++++++++++++++++++++++
		bool RECURSIVE =				true;
		bool RECURSIVE_WITH_OUTPUT =	false;
		bool SINGLE_FIT =				false;
		bool COMPUTE =					false;
		bool RENDER =					false;

		string inputName =	"image";
		int reduction =		10000;
		//++++++++++++++++++++++++++++


		string inputImage = "images/" + inputName + ".png";
		string ellipseImage = "se_ronin_" + inputName + "_ellipseImage.png";

		int width;
		int height;

		if (RECURSIVE) {

			auto startTime = chrono::high_resolution_clock::now();


			//load the pixels from image
			int* pixelGrid = image_input::loadPixelsFromImage(inputImage, width, height);
			if (pixelGrid == nullptr) {
				return -1;
			}


			////extract the contours of the image using the own reduce function
			//vector<int> contourVector = util::getContoursArray(pixelGrid, width, height);
			//int no_contourPixels = int(contourVector.size() / 2);
			//int* contourList = &contourVector[0];
			//


			//extract the contours of the image using OpenCV (implicitly perform connected components separation)
			vector<vector<cv::Point>> contoursPointsVector = image_input::getContoursFromImage(inputImage, width, height);

			int contoursCount = contoursPointsVector.size();
			auto preCalculateTime = chrono::high_resolution_clock::now();
			vector<vector<double>> totalEllipsesVector;

			//for each contour:
			for (int i = 0; i < contoursCount; i++) {

				//transform the contoursPoints to a contourVector
				vector<cv::Point> contourPoints = contoursPointsVector.at(i);
				vector<int> contourVector;
				for (int j = 0; j < contourPoints.size(); j++) {
					contourVector.push_back(contourPoints.at(j).x);
					contourVector.push_back(contourPoints.at(j).y);
				}

				//draw the contour
				//image_output::pixelVectorToImage(contourVector, width, height, to_string(i) + "contour.png");

				int* pixelGrid = util::getFilledPixelGridFromContour(contourPoints, width, height);

				//use Ronin to fit superellipses to the contour
				vector<vector<double>> contourEllipsesVector = useRoninRecursive(pixelGrid, contourVector, width, height);
				contourVector.clear();

				//draw the fitted superellipses
				//int err = processSuperellipsesFromVector(contourEllipsesVector, to_string(i) + ellipseImage, width, height);

				//add the superellipses of this contour to the vector of all superellipses
				totalEllipsesVector.insert(totalEllipsesVector.end(), contourEllipsesVector.begin(), contourEllipsesVector.end());
			}




			////recursively fit superellipses to the contours
			//vector<vector<double>> ellipsesVector = useRoninRecursive(pixelGrid, contourVector, width, height);
			auto postCalculateTime = chrono::high_resolution_clock::now();


			//render the fitted superellipses to an image file
			int err = processSuperellipsesFromVector(totalEllipsesVector, ellipseImage, width, height);
			auto postRenderTime = chrono::high_resolution_clock::now();



			//time measurement
			auto prepareDuration = preCalculateTime - startTime;
			auto calculateDuration = postCalculateTime - preCalculateTime;
			auto renderDuration = postRenderTime - postCalculateTime;
			cout << "prepareDuration:     " << double(chrono::duration_cast<chrono::microseconds>(prepareDuration).count())/1000000.0 << "s" << endl;
			cout << "calculateDuration:   " << double(chrono::duration_cast<chrono::microseconds>(calculateDuration).count())/1000000.0 << "s" << endl;
			cout << "renderDuration:      " << double(chrono::duration_cast<chrono::microseconds>(renderDuration).count())/1000000.0 << "s" << endl;
		}



		if (RECURSIVE_WITH_OUTPUT) {
			//load the pixels from image
			int* pixelGrid = image_input::loadPixelsFromImage(inputImage, width, height);


			//extract the contours of the image
			vector<int> contourVector = util::getContoursArray(pixelGrid, width, height);
			int no_contourPixels = int(contourVector.size() / 2);
			int* contourList = &contourVector[0];


		}




	}









	//#######################################################
	//##########        Superellipses Ceres        ##########       
	//#######################################################

	if (USE_SUPERELLIPSES_CERES) {

		//++++++++++++++++++++++++++++
		bool PREPARE =			true;
		bool COMPUTE =			true;
		bool RENDER =			true;

		string inputName = "ellipsetest8";
		int reduction = 100;
		//++++++++++++++++++++++++++++

		string inputImage = "images/" + inputName + ".png";
		string reducedImgName = "se_ceres_" + inputName + "_reducedImage.png";
		string pixelFile = "se_ceres_" + inputName + "_pixels.txt";
		string ellipseTextFile = "se_ceres_" + inputName + "_textellipses";
		string ellipseImage = "se_ceres_" + inputName + "_ellipseImage.png";


		if (PREPARE) {
			cout << "Preparing input for superellipses - Ceres" << endl;
			//int err = prepareInputForCeres(inputImage, reducedImgName, pixelFile, reduction);
			cout << "Prepared input from file " << inputImage << " to file " << pixelFile << endl;
		}


		if (COMPUTE) {
			//double x, y, theta;
			//useCeresRecursive("allPixels", pixelFile, ellipseTextFile, "", "globalEllipses");

		}


		if (RENDER) {
			cout << "Rendering superellipses" << endl;
			//int err = processSuperellipsesFromTextfileCeres("globalEllipses", "globalEllipses.png");
			cout << "Rendered ellipses from file " << ellipseTextFile << " to Image " << ellipseImage << endl;

		}




	}



}


