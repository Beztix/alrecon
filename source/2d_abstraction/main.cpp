/**
*************************************************************************
*
* @file main.cpp
*
* Main class for executing the various algorithms of alrecon.
* Supports Execution of:
* - alpha shapes with CGAL, a non-parallel implementation of the alpha shape algorithm in the computational geometry algorithms library
* - alpha shapes with GPUAlpha, an implementation of the alpha shape algorithm on the GPU
* - superellipses with ceres, an implementation of superellipse fitting using the non-linear least squares solver ceres
* - superellipses rosin, an implementation of superellipse fitting using a simple fit-algorithm developed by Paul Rosin
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
#include "se_util.h"
#include "tree.hh"
#include "tree_util.hh"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


// CGAL stuff
#include "as_cgal.h"


// GPUAlpha stuff
#include "as_gpu_input.h"


// Superellipses with ceres
#include <ceres/ceres.h>


// Superellipses with rosin
#include "se_rendering.h"
#include "se_rosin_core.h"
#include "se_rosin_recursive.h"
#include "se_rosin_tree.h"
#include "se_split.h"
#include "convexHull.h"
#include "convexHullForC.h"


#include <Windows.h>


using namespace std;



int main() {


	Sleep(1000);

	bool USE_CGAL =					false;
	bool USE_GPUALPHA =				false;
	bool USE_SUPERELLIPSES_ROSIN =	true;
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
	//##########        Superellipses Rosin        ##########       
	//#######################################################

	if (USE_SUPERELLIPSES_ROSIN) {

		//++++++++++++++++++++++++++++
		bool RECURSIVE =	false;
		bool TREE =			true;

		string inputName =		"image";
		int quality =			200;
		vector<int> qualityValues =	{ 50000 , 10000 , 1000 , 200 ,};
		int iterations =		100;
		//++++++++++++++++++++++++++++

		string inputImage = "images/" + inputName + ".png";

		int width;
		int height;

		LARGE_INTEGER frequency;        // ticks per second
		LARGE_INTEGER t1, t2, t3, t4, t5, t6;	//ticks
		QueryPerformanceFrequency(&frequency);
		double totalComputeDuration = 0;

		//load the contours from the image
		QueryPerformanceCounter(&t1);
		vector<vector<cv::Point>> contours = image_input::getContoursFromImage(inputImage, width, height);

		QueryPerformanceCounter(&t2);
		double prepareDuration = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
		cout << "prepareDuration:     " << prepareDuration << "ms" << endl;



		if (RECURSIVE) {
			string ellipseImage = "se_rosin_rec_" + inputName + "_ellipseImage_" + to_string(quality) + ".png";
			string combinedImage = "se_rosin_rec_" + inputName + "_combined" + to_string(quality) + ".png";

			vector<se::superellipse> totalEllipsesVector;

			//do the whole computation multiple times (performance averageing)
			for (int i = 0; i < iterations; i++) {
				totalEllipsesVector.clear();

				//use recursive rosin fitting to fit superellipses to the contours

				QueryPerformanceCounter(&t3);
				totalEllipsesVector = startRosinRecursive(contours, width, height, quality);

				QueryPerformanceCounter(&t4);
				double computeDuration = (t4.QuadPart - t3.QuadPart) * 1000.0 / frequency.QuadPart;
				cout << "computeDuration:     " << computeDuration << "ms" << endl;

				totalComputeDuration = totalComputeDuration + computeDuration;
			}

			cout << endl;
			cout << "totalComputeDuration:         " << totalComputeDuration << "ms" << endl;
			cout << "medium compute Duration:      " << totalComputeDuration / iterations << "ms" << endl;

			cout << endl;
			cout << "quality used:                 " << quality << endl;
			cout << "number of superellipses:      " << totalEllipsesVector.size() << endl;

			//render the fitted superellipses to an image file
			QueryPerformanceCounter(&t5);
			int err = processSuperellipsesFromVector(totalEllipsesVector, ellipseImage, width, height);

			//combine the original and the ellipse image
			image_output::combineOriginalAndEllipseImage(inputImage, ellipseImage, combinedImage);

			QueryPerformanceCounter(&t6);
			double renderDuration = (t6.QuadPart - t5.QuadPart) * 1000.0 / frequency.QuadPart;
			cout << "renderDuration:               " << renderDuration << "ms" << endl;
		}



		if (TREE) {
			string ellipseImagePart = "se_rosin_tree_" + inputName + "_ellipseImage_";
			string combinedImagePart = "se_rosin_tree_" + inputName + "_combined_";

			tree<se::contourAndSe> seTree;

			//do the whole computation multiple times (performance averaging)
			for (int i = 0; i < iterations; i++) {
				seTree.clear();

				QueryPerformanceCounter(&t3);
		
				//add root node to the tree
				tree<se::contourAndSe>::iterator top = seTree.begin();
				se::contourAndSe rootEllipse = se::contourAndSe();
				seTree.insert(top, rootEllipse);
				
				//cout << endl;
				//cout << "initial tree" << endl;
				//kptree::print_tree_bracketed(seTree);
				//cout << endl;

				startRosinTree(seTree, contours, width, height, qualityValues);

				//cout << endl;
				//cout << "final tree" << endl;
				//kptree::print_tree_bracketed(seTree);
				//cout << endl;

				QueryPerformanceCounter(&t4);
				double computeDuration = (t4.QuadPart - t3.QuadPart) * 1000.0 / frequency.QuadPart;
				cout << "computeDuration:     " << computeDuration << "ms" << endl;

				totalComputeDuration = totalComputeDuration + computeDuration;
			}

			int depth = seTree.max_depth();

			cout << endl;
			cout << "totalComputeDuration:         " << totalComputeDuration << "ms" << endl;
			cout << "medium compute Duration:      " << totalComputeDuration / iterations << "ms" << endl;

			cout << endl;
			cout << "quality used:                 " << quality << endl;
			cout << "depth of tree:                " << depth << endl;

			//render the fitted superellipses to image files
			QueryPerformanceCounter(&t5);

			for (int k = 1; k <= depth; k++) {
				vector<se::superellipse> ellipses = se::getEllipsesOfGivenDepth(seTree, k);
				processSuperellipsesFromVector(ellipses, ellipseImagePart + to_string(k) + ".png", width, height);

				//combine the original and the ellipse image
				image_output::combineOriginalAndEllipseImage(inputImage, ellipseImagePart + to_string(k) + ".png", combinedImagePart + to_string(k) + ".png");
			}

		

			QueryPerformanceCounter(&t6);
			double renderDuration = (t6.QuadPart - t5.QuadPart) * 1000.0 / frequency.QuadPart;
			cout << "renderDuration:               " << renderDuration << "ms" << endl;




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


