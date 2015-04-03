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


	//Sleep(10000);

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

			LARGE_INTEGER frequency;        // ticks per second
			LARGE_INTEGER t1, t2, t3, t4;	//ticks
			// get ticks per second
			QueryPerformanceFrequency(&frequency);


			//load the separate connected components from the image
			QueryPerformanceCounter(&t1);
			vector<vector<cv::Point2i>> blobs = image_input::getBlobsFromImage(inputImage, width, height);


			//fit superellipses for each connected component:
			QueryPerformanceCounter(&t2);
			vector<vector<double>> totalEllipsesVector;
			for (int i = 0; i < blobs.size(); i++) {
				vector<cv::Point2i> currentBlob = blobs.at(i);

				//determine dimension and offset of the connected component
				int minX = INT_MAX;
				int maxX = INT_MIN; 
				int minY = INT_MAX;
				int maxY = INT_MIN;
				for (int j = 0; j < currentBlob.size(); j++) {
					cv::Point2i currentPoint = currentBlob.at(j);
					if (currentPoint.x < minX) {
						minX = currentPoint.x;
					}
					if (currentPoint.x > maxX) {
						maxX = currentPoint.x;
					}
					if (currentPoint.y < minY) {
						minY = currentPoint.y;
					}
					if (currentPoint.y > maxY) {
						maxY = currentPoint.y;
					}
				}
				int sizeX = maxX - minX + 3;
				int sizeY = maxY - minY + 3;
				int offsetX = minX - 1;
				int offsetY = minY - 1;


				//create a pixelGrid and a Mat from the blob points
				int* pixelGrid = new int[sizeX * sizeY];
				fill_n(pixelGrid, sizeX*sizeY, 0);
				cv::Mat blobMat = cv::Mat(sizeY, sizeX, CV_8UC3);
				blobMat.setTo(cv::Scalar(0, 0, 0));
				for (int j = 0; j < currentBlob.size(); j++) {
					//add the points of the connected component to the pixelGrid / Mat
					cv::Point2i currentPoint = currentBlob.at(j);
					int x = currentPoint.x - offsetX;
					int y = currentPoint.y - offsetY;
					pixelGrid[y*sizeX + x] = 200;
					blobMat.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
				}

				//extract the contour of the connected component
				vector<vector<cv::Point>> contours;
				cv::Mat gray;
				cv::cvtColor(blobMat, gray, CV_BGR2GRAY);
				cv::findContours(gray, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
				vector<cv::Point> contourPoints = contours.at(0);
				vector<int> contourVector;
				for (int j = 0; j < contourPoints.size(); j++) {
					contourVector.push_back(contourPoints.at(j).x);
					contourVector.push_back(contourPoints.at(j).y);
				}

				//use Ronin fitting to fit superellipses to the contour
				vector<vector<double>> contourEllipsesVector = useRoninRecursive(pixelGrid, contourVector, sizeX, sizeY, 50);
				contourVector.clear();

				//draw the fitted superellipses
				//int err = processSuperellipsesFromVector(contourEllipsesVector, to_string(i) + ellipseImage, width, height);


				//add the offset of the connected components to the calculated ellipses
				for (int j = 0; j < contourEllipsesVector.size(); j++) {
					contourEllipsesVector[j][0] = contourEllipsesVector[j][0] + offsetX;
					contourEllipsesVector[j][1] = contourEllipsesVector[j][1] + offsetY;
				}

				//add the superellipses of this contour to the vector of all superellipses
				totalEllipsesVector.insert(totalEllipsesVector.end(), contourEllipsesVector.begin(), contourEllipsesVector.end());
			}
			QueryPerformanceCounter(&t3);


			//render the fitted superellipses to an image file
			int err = processSuperellipsesFromVector(totalEllipsesVector, ellipseImage, width, height);
			QueryPerformanceCounter(&t4);


			//time measurement
			double prepareDuration = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
			double computeDuration = (t3.QuadPart - t2.QuadPart) * 1000.0 / frequency.QuadPart;
			double renderDuration = (t4.QuadPart - t3.QuadPart) * 1000.0 / frequency.QuadPart;
			cout << "prepareDuration:     " << prepareDuration << "ms" << endl;
			cout << "calculateDuration:   " << computeDuration << "ms" << endl;
			cout << "renderDuration:      " << renderDuration << "ms" << endl;

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



	//Sleep(10000);
}


