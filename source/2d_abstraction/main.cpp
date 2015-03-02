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
#include <ceres/ceres.h>
#include "se_split.h"




#include "convexHull.h"


using namespace std;
using ceres::Solver;



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







void appendEllipseToGlobalFile(string ellipseTextFile, string globalEllipseTextFile) {
	ifstream infile(ellipseTextFile);
	int width, height;
	string dummy;
	infile >> dummy >> width;
	infile >> dummy >> height;


	//Test if global ellipse file is initialized
	ifstream readOutfile(globalEllipseTextFile);
	int dummyInt;
	if (!(readOutfile >> dummy >> dummyInt)) {
		ofstream outfileInit(globalEllipseTextFile);
		outfileInit << "w " << width << endl;
		outfileInit << "h " << height  << endl;
	}


	//append ellipse to global ellipse file
	double xc, yc, theta, a, b, epsilon;
	infile >> xc >> yc >> theta >> a >> b >> epsilon;

	ofstream outfile(globalEllipseTextFile, std::ios_base::app);
	outfile << xc << " " << yc << " " << theta << " " << a << " " << b << " " << epsilon << endl;


}



void useCeresRecursive(string pixelFile, string contourFile, string ellipseTextFile, string id, string globalEllipseTextFile) {
	double x, y, theta;
	double maxFinalCost = useCeres(contourFile, ellipseTextFile, x, y, theta);
	if (maxFinalCost > 10000) {
		string id1 = id + "0";
		string id2 = id + "1";
		splitImage(pixelFile, x, y, theta, "file" + id1, "file" + id2);
		
		pixelFileToImage("file" + id1, "file" + id1 + ".png");
		pixelFileToImage("file" + id2, "file" + id2 + ".png");
		
		getContours("file" + id1, "contourFile" + id1);
		getContours("file" + id2, "contourFile" + id2);

	
		pixelFileToImage("contourFile" + id1, "contourFile" + id1 + ".png");
		pixelFileToImage("contourFile" + id2, "contourFile" + id2 + ".png");

		useCeresRecursive("file" + id1, "contourFile" + id1, "ellipseFile" + id1, id1, globalEllipseTextFile);
		processSuperellipsesFromTextfileCeres("ellipseFile" + id1, "ellipseFile" + id1 + ".png");

		useCeresRecursive("file" + id2, "contourFile" + id2, "ellipseFile" + id2, id2, globalEllipseTextFile);
		processSuperellipsesFromTextfileCeres("ellipseFile" + id2, "ellipseFile" + id2 + ".png");

	}
	else {
		appendEllipseToGlobalFile(ellipseTextFile, globalEllipseTextFile);
	}


}








int main() {



	bool USE_CGAL =					false;
	bool USE_GPUALPHA =				false;
	bool USE_SUPERELLIPSES_RONIN =	true;
	bool USE_SUPERELLIPSES_CERES =	false;



	//vector<Point> points;
	//Point p;

	//p.x = 1;
	//p.y = 1;
	//points.emplace_back(p);
	//p.x = 1;
	//p.y = 3;
	//points.emplace_back(p);
	//p.x = 2;
	//p.y = 1;
	//points.emplace_back(p);
	//p.x = 2;
	//p.y = 2;
	//points.emplace_back(p);
	//p.x = 3;
	//p.y = 2;
	//points.emplace_back(p);
	//p.x = 3;
	//p.y = 3;
	//points.emplace_back(p);
	//p.x = 4;
	//p.y = 1;
	//points.emplace_back(p);
	//p.x = 4;
	//p.y = 4;
	//points.emplace_back(p);
	//p.x = 0;
	//p.y = 0;
	//points.emplace_back(p);

	//
	//
	//vector<Point> hull = convex_hull(points);

	//for (int i = 0; i < hull.size(); i++) {
	//	Point p = hull.at(i);
	//	cout << p.x << " " << p.y << endl;
	//}

	


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
		bool RESTORE_IMAGE =	false;
		bool PREPARE =			false;
		bool COMPUTE =			true;
		bool RENDER =			true;

		string inputName =	"mouse_part_sorted";
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


		//sortPixelFile("mouse_part__.txt", "mouse_part_sorted.txt");


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

		string inputName = "ellipsetest8";
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
			double x, y, theta;
			

			useCeresRecursive("allPixels", pixelFile, ellipseTextFile, "", "globalEllipses");



		}


		if (RENDER) {
			cout << "Rendering superellipses" << endl;
			int err = processSuperellipsesFromTextfileCeres("globalEllipses", "globalEllipses.png");
			cout << "Rendered ellipses from file " << ellipseTextFile << " to Image " << ellipseImage << endl;

			//pixelFileToImage("file0", "file0neu.png");
			/*pixelFileToImage("file00", "file00.png");
			pixelFileToImage("file01", "file01.png");
			pixelFileToImage("file1", "file1.png");
			pixelFileToImage("file01", "file01.png");
			pixelFileToImage("file010", "file010.png");
			pixelFileToImage("file011", "file011.png");
			pixelFileToImage("file0100", "file0100.png");
			pixelFileToImage("file0101", "file0101.png");

			processSuperellipsesFromTextfileCeres("ellipseFile0", "ellipseFile0.png");
			processSuperellipsesFromTextfileCeres("ellipseFile1", "ellipseFile1.png");
			processSuperellipsesFromTextfileCeres("ellipseFile00", "ellipseFile00.png");
			processSuperellipsesFromTextfileCeres("ellipseFile01", "ellipseFile01.png");
			processSuperellipsesFromTextfileCeres("ellipseFile010", "ellipseFile010.png");
			processSuperellipsesFromTextfileCeres("ellipseFile011", "ellipseFile011.png");
			processSuperellipsesFromTextfileCeres("ellipseFile0100", "ellipseFile0100.png");
			processSuperellipsesFromTextfileCeres("ellipseFile0101", "ellipseFile0101.png");*/

		}




	}



}


