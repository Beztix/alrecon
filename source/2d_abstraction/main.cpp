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

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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



#include <Windows.h>

using namespace std;





int main() {


	Sleep(1000);

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
		int quality =		 3000;
		//++++++++++++++++++++++++++++


		string inputImage = "images/" + inputName + ".png";
		string ellipseImage = "se_ronin_" + inputName + "_ellipseImage.png";
		string combinedImage = "se_ronin_" + inputName + "_combined.png";

		int width;
		int height;


		
		

		if (RECURSIVE) {

			LARGE_INTEGER frequency;        // ticks per second
			LARGE_INTEGER t1, t2, t3, t4;	//ticks
			QueryPerformanceFrequency(&frequency);

			vector<vector<double>> totalEllipsesVector;


			double totalComputeDuration = 0;
			int iterations = 100;

			
			//------------------------------------------------------------
			//    PERFORMANCE TESTING: DO THE SAME STUFF 100 TIMES
			//------------------------------------------------------------
			for (int i = 0; i < iterations; i++) {
				totalEllipsesVector.clear();

				//load the contours from the image
				QueryPerformanceCounter(&t1);
				vector<vector<cv::Point>> contours = image_input::getContoursFromImage(inputImage, width, height);

				QueryPerformanceCounter(&t2);
				double prepareDuration = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
				cout << "prepareDuration:     " << prepareDuration << "ms" << endl;


				//for each contour:
				for (int i = 0; i < contours.size(); i++) {
					vector<cv::Point> currentContour = contours.at(i);

					//determine dimension and offset of the contour
					int minX = INT_MAX;
					int maxX = INT_MIN;
					int minY = INT_MAX;
					int maxY = INT_MIN;
					for (int j = 0; j < currentContour.size(); j++) {
						cv::Point currentPoint = currentContour.at(j);
						if (currentPoint.x < minX) minX = currentPoint.x;
						if (currentPoint.x > maxX) maxX = currentPoint.x;
						if (currentPoint.y < minY) minY = currentPoint.y;
						if (currentPoint.y > maxY) maxY = currentPoint.y;
					}
					int sizeX = maxX - minX + 3;
					int sizeY = maxY - minY + 3;
					int offsetX = minX - 1;
					int offsetY = minY - 1;


					//substract the offset from the contour points
					vector<cv::Point> currentContourWithOffset(currentContour.size());
					for (int j = 0; j < currentContour.size(); j++) {
						cv::Point currentPoint = currentContour.at(j);
						cv::Point newPoint = cv::Point(currentPoint.x - offsetX, currentPoint.y - offsetY);
						currentContourWithOffset.at(j) = newPoint;
					}



					//use recursive Ronin fitting to fit superellipses to the contour
					vector<vector<double>> contourEllipsesVector = useRoninRecursive(currentContourWithOffset, offsetX, offsetY, sizeX, sizeY, quality);
					

					


					//draw the fitted superellipses
					//int err = processSuperellipsesFromVector(contourEllipsesVector, to_string(i) + ellipseImage, width, height);


					//add the superellipses of this contour to the vector of all superellipses
					totalEllipsesVector.insert(totalEllipsesVector.end(), contourEllipsesVector.begin(), contourEllipsesVector.end());
				}
				QueryPerformanceCounter(&t3);
				double computeDuration = (t3.QuadPart - t2.QuadPart) * 1000.0 / frequency.QuadPart;
				cout << "computeDuration:   " << computeDuration << "ms" << endl;
				totalComputeDuration = totalComputeDuration + computeDuration;

			}

			QueryPerformanceCounter(&t3);
			//render the fitted superellipses to an image file
			int err = processSuperellipsesFromVector(totalEllipsesVector, ellipseImage, width, height);

			//combine the original and the ellipse image
			image_output::combineOriginalAndEllipseImage(inputImage, ellipseImage, combinedImage);

			QueryPerformanceCounter(&t4);
			double renderDuration = (t4.QuadPart - t3.QuadPart) * 1000.0 / frequency.QuadPart;
			cout << "renderDuration:      " << renderDuration << "ms" << endl;


			cout << endl;
			cout << "medium compute Duration:    " << totalComputeDuration / iterations << "ms" << endl;

			




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



	Sleep(1000);
}


