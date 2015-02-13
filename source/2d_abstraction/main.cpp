#include <string>




// CGAL stuff
#include "as_cgal.h"



// GPUAlpha stuff



// Superellipses stuff
#include "se_rendering.h"
#include "se_input.h"





using namespace std;



int main() {

	bool USE_CGAL =					true;
	bool USE_GPUALPHA =				false;
	bool PREPARE_SUPERELLIPSES =	false;
	bool RENDER_SUPERELLIPSES =		false;

	

	//==========================
	//====       CGAL       ====
	//==========================
	if (USE_CGAL) {

		//++++++++++++++++++++++++++++
		string inputImage = "images/image.png";
		//++++++++++++++++++++++++++++


		useCgal(inputImage);
	}




	//==========================
	//====     GPUAlpha     ====
	//==========================
	if (USE_GPUALPHA) {

		//++++++++++++++++++++++++++++

		//++++++++++++++++++++++++++++

	}



	//===========================
	//====   Superellipses   ====
	//===========================
	if (PREPARE_SUPERELLIPSES) {

		//++++++++++++++++++++++++++++
		string inputImage = "images/image.png";
		string pixelFile = "image.pix";
		//++++++++++++++++++++++++++++

		cout << "Preparing superellipses" << endl;


		int err = prepareInput(inputImage, pixelFile);
		cout << "Prepared input from file " << inputImage << " to file " << pixelFile << endl;





	}

	if (RENDER_SUPERELLIPSES) {

		//++++++++++++++++++++++++++++
		string inputFile = "image_output";
		//++++++++++++++++++++++++++++

		cout << "Rendering superellipses" << endl;


		int err = processSuperellipsesFromTextfile(inputFile);
		


	}



}