#include <string>




// CGAL stuff
#include "as_cgal.h"



// GPUAlpha stuff
#include "as_gpu_input.h"


// Superellipses stuff
#include "se_rendering.h"
#include "se_input.h"
#include "se_epsilon_new.h"






using namespace std;



int main() {



	bool USE_CGAL =				true;
	bool USE_GPUALPHA =			true;
	bool USE_SUPERELLIPSES =	true;


	


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

		string inputName =	"image";
		int reduction =		10; 
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
	if (USE_SUPERELLIPSES) {

		//++++++++++++++++++++++++++++
		bool PREPARE =	true;
		bool COMPUTE =	true;
		bool RENDER =	true;

		string inputName =	"image";
		int reduction =		15;
		//++++++++++++++++++++++++++++

		string inputImage = "images/" + inputName + ".png";
		string reducedImgName = "se_" + inputName + "_reducedImage.png";
		string pixelFile = "se_" + inputName + "_pixels.txt";
		string ellipseTextFile = "se_" + inputName + "_textellipses";
		string ellipseImage = "se_" + inputName + "_ellipseImage.png";


		if (PREPARE) {
			cout << "Preparing input for superellipses" << endl;
			int err = prepareInputForSE(inputImage, reducedImgName, pixelFile, reduction);
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

			calculateEllipse(pixelFileChar, ellipseTextFileChar);
			cout << "Calculated ellipses for file " << pixelFile << " to file " << ellipseTextFile << endl;
		}


		if (RENDER) {
			cout << "Rendering superellipses" << endl;
			int err = processSuperellipsesFromTextfile(ellipseTextFile, ellipseImage);
			cout << "Rendered ellipses from file " << ellipseTextFile << " to Image " << ellipseImage << endl;
		}

	}



}