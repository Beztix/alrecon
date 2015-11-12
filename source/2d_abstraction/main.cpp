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

#define NOGDI


#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <conio.h>
#include <Windows.h>


// Utility
#include "image_input.h"
#include "image_output.h"
#include "text_input.h"
#include "util.h"
#include "tree.hh"
#include "tree_util.hh"


// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


// Viral
#include <viral_core/engine.hpp>
#include <viral_core/window.hpp>
#include <viral_core/input.hpp>
#include <viral_core/file.hpp>
#include <viral_core/timer.hpp>
#include <viral_core/image.hpp>
#include <viral_core/render_resource.hpp>
#include <viral_core/render_scene.hpp>
#include <viral_core/render_process.hpp>
#include <viral_core/render_command.hpp>
#include <viral_core/render_task.hpp>
#include <viral_core/render.hpp>


// Superellipses with rosin
#include "se_rendering.h"
#include "se_rosin_core.h"
#include "se_rosin_tree.h"
#include "se_split.h"
#include "se_util.h"
#include "convexHull.h"
#include "convexHullForC.h"


// Reconstruction 
#include "rec_reconstruction_trivial.h"
#include "rec_reconstruction_frustum.h"
#include "rec_rendering.h"
#include "rec_sampling.h"
#include "rec_frustum_intersection.h"
#include "rec_frustum.h"
#include "rec_file_camera.h"
#include "rec_sensor.h"





int main() {




	Sleep(2000);

	int NROFCAMS = 7;
	bool DEBUG = false;
	bool RENDER_IMAGES = false;
	
	


	//#######################################################
	//##########        Superellipses Rosin        ##########       
	//#######################################################

	cout << "=======  Starting superellipses computation using Rosin  =======" << endl;
	cout << endl;
	cout << endl;
	cout << endl;

	std::vector<tree<rec::seAndFrust>> seAndFrustTrees;
	std::vector<tree<se::contourAndSe>> contourAndSeTrees;



	// for each camera
	for (int cam = 1; cam <= NROFCAMS; cam++) {


		//++++++++++++++++++++++++++++
		string inputName = "occMask_" + std::to_string(cam);
		string inputImage = "images/" + inputName + ".png";
		string ellipseImagePart = "se_rosin_tree_" + inputName + "_ellipseImage_";
		string bbImagePart = "se_rosin_tree_" + inputName + "_bb_";
		string combinedImagePart = "se_rosin_tree_" + inputName + "_combined_";

		int quality = 200;
		vector<int> qualityValues = { 50000, 10000, 1000, 200, 100 };
		int iterations = 1;
		//++++++++++++++++++++++++++++


		cout << "===   IMAGE: " << inputName << ".png   ===" << endl;

		int width;
		int height;

		LARGE_INTEGER frequency;        // ticks per second
		LARGE_INTEGER t1, t2, t3, t4, t5, t6;	//ticks
		QueryPerformanceFrequency(&frequency);
		double totalComputeDuration = 0;



		// ===========   PREPARATION   ===========

		//load the contours from the image
		QueryPerformanceCounter(&t1);
		vector<vector<cv::Point>> contours = image_input::getContoursFromImage(inputImage, width, height);

		QueryPerformanceCounter(&t2);
		double prepareDuration = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
		cout << "prepareDuration:     " << prepareDuration << "ms" << endl;




		// ===========   SE COMPUTATION   ===========

		tree<se::contourAndSe> contourAndSeTree;
		tree<rec::seAndFrust> seAndFrustTree;

		//do the whole computation multiple times (performance averaging)
		for (int i = 0; i < iterations; i++) {
			contourAndSeTree.clear();
			seAndFrustTree.clear();


			QueryPerformanceCounter(&t3);

			//add root node to the contourAndSeTree (empty contourAndSe)
			tree<se::contourAndSe>::iterator contourAndSeTreeTop = contourAndSeTree.begin();
			se::contourAndSe rootContourAndSe = se::contourAndSe();
			contourAndSeTree.insert(contourAndSeTreeTop, rootContourAndSe);

			//add root node to the seAndFrustTree (seAndFrust containing the whole image as bounding box)
			tree<rec::seAndFrust>::iterator seAndFrustTreeTop = seAndFrustTree.begin();
			viral_core::vector2f corner1((float)width, 0.f);
			viral_core::vector2f corner2(0.f, 0.f);
			viral_core::vector2f corner3(0.f, (float)height);
			viral_core::vector2f corner4((float)width, (float)height);
			rec::seAndFrust rootSeAndFrust(corner1, corner2, corner3, corner4);
			seAndFrustTree.insert(seAndFrustTreeTop, rootSeAndFrust);


			if (DEBUG) {
				cout << endl;
				cout << "initial setTree" << endl;
				kptree::print_tree_bracketed(contourAndSeTree);
				cout << endl;
				cout << "initial seAndFrustTree" << endl;
				kptree::print_tree_bracketed(seAndFrustTree);
				cout << endl;
			}

			//start the main computation
			startRosinTree(contourAndSeTree, seAndFrustTree, contours, width, height, qualityValues);

			if (DEBUG) {
				cout << endl;
				cout << "final seTree" << endl;
				kptree::print_tree_bracketed(contourAndSeTree);
				cout << endl;
				cout << "final seAndFrustTree" << endl;
				kptree::print_tree_bracketed(seAndFrustTree);
				cout << endl;
			}

			//add the trees of the current camera to the lsit of all trees
			//contourAndSeTrees.push_back(contourAndSeTree);
			//seAndFrustTrees.push_back(seAndFrustTree);

			QueryPerformanceCounter(&t4);
			double computeDuration = (t4.QuadPart - t3.QuadPart) * 1000.0 / frequency.QuadPart;
			cout << "computeDuration:     " << computeDuration << "ms" << endl;
			totalComputeDuration = totalComputeDuration + computeDuration;
		}



		// ===========   OUTPUT   ===========

		int depth = contourAndSeTree.max_depth();

		cout << endl;
		cout << "totalComputeDuration:         " << totalComputeDuration << "ms" << endl;
		cout << "medium compute Duration:      " << totalComputeDuration / iterations << "ms" << endl;
		cout << endl;
		cout << "quality used:                 " << quality << endl;
		cout << "depth of tree:                " << depth << endl;



		if (RENDER_IMAGES) {

			//render the fitted superellipses to image files
			QueryPerformanceCounter(&t5);

			for (int k = 1; k <= depth; k++) {
				vector<se::superellipse> ellipses = se::getEllipsesOfGivenDepth(contourAndSeTree, k);
				processSuperellipsesFromVector(ellipses, ellipseImagePart + to_string(k), width, height);
				processSuperellipsesToBoundingBoxFromVector(ellipses, bbImagePart + to_string(k), width, height, 300);
				image_output::combineOriginalAndEllipseImage(inputImage, ellipseImagePart + to_string(k) + ".png", combinedImagePart + to_string(k) + ".png");
			}


			QueryPerformanceCounter(&t6);
			double renderDuration = (t6.QuadPart - t5.QuadPart) * 1000.0 / frequency.QuadPart;
			cout << "renderDuration:               " << renderDuration << "ms" << endl;
			cout << endl;
		}
	}	
	// end of for each camera













	//#######################################################
	//##########          Reconstruction           ##########       
	//#######################################################

	cout << "=======  Starting reconstruction computation  =======" << endl;
	cout << endl;
	cout << endl;
	cout << endl;


	//rec::doAllSamplingCalculations();

	std::vector<viral_core::vector> cameraPositions;
	std::vector<std::vector<std::vector<viral_core::vector>>> directionsGrids;

	std::vector<rec::file_camera> cameras;
	std::vector<rec::sensor> sensors;


	cout << "Initialising file_camera and sensor objects, loading directionsGrids" << endl;

	// for each camera
	for (int cam = 1; cam <= NROFCAMS; cam++) {

		// create file_camera and sensor objects for camera transformations
		rec::file_camera camera(cam);
		rec::sensor sens(cam, camera.image_size, rec::sensor::projection_pinhole_distort);
		sens.set_pinhole_distort(camera.world_to_device_pinhole_distort_, camera.pinhole_distort_center_, camera.pinhole_distort_focus_, 
			camera.distort_r1_, camera.distort_r2_, camera.distort_t1_, camera.distort_t2_);
		cameras.push_back(camera);
		sensors.push_back(sens);

		cameraPositions.push_back(camera.cam_position_);

		// load directionsGrids
		std::string locationStringDirections = "../../assets/camera_inversion/offset_300/directions_distanceNormalized_cam" + std::to_string(cam);
		std::vector<std::vector<viral_core::vector>> directionsGrid = text_input::readPositionsGridFromBinaryfile(locationStringDirections + ".bin", 1240, 1080);


		directionsGrids.push_back(directionsGrid);

	}


	//rec::createObject3DTree(cameraPositions, directionsGrids, seAndFrustTrees, 7, 640, 480, 300);

	//std::vector<viral_core::vector> occupiedWorldPositions = rec::reconstruct_trivial(20);

	//rec::renderOccupiedPositions(occupiedWorldPositions, 10, 0.1f);











	Sleep(2000);

}


