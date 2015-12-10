/**
*************************************************************************
*
* @file text_input.cpp
*
* 
*
************************************************************************/

#include "text_input.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


using namespace std;


namespace text_input {



	std::vector<viral_core::vector> readVectorListFromTextFile(std::string input) {
		std::vector<viral_core::vector> result;

		ifstream infile(input);
		double x, y, z;

		while (infile >> x >> y >> z) {
			result.push_back(viral_core::vector(x, y, z));
		}

		infile.close();
		return result;
	}



	/// reads a list of pixels from a text file in ronin format, width and height are estimated by pixel position
	/**
	*
	*/

	vector<int> loadPixelsFromRoninTextfile(string input, int &width, int &height) {
		vector<int> result;
		ifstream infile(input);
		string line;
		int i;
		// reading the first two lines (not needed here)
		infile >> line;
		infile >> line >> i;
		int x, y;
		int extractedWidth = 0;
		int extractedHeight = 0;
		// reading pixels from textfile
		while (infile >> x >> y) {
			if (x == -1) break;
			if (x > extractedWidth) extractedWidth = x;
			if (y > extractedHeight) extractedHeight = y;
			result.push_back(x);
			result.push_back(y);
		}
		infile.close();
		extractedWidth += 10;
		extractedHeight += 10;
		width = extractedWidth;
		height = extractedHeight;

		return result;
	}




	/// reads a list of pixels from a text file in beztix format, width and height are read from file
	/**
	*
	*/

	vector<int> loadPixelsFromBeztixTextfile(string input, int &width, int &height) {
		vector<int> result;
		ifstream infile(input);
		string dummy;
		// reading width & height
		infile >> dummy >> width;
		infile >> dummy >> height;
		int x, y;
		// reading pixels from textfile
		while (infile >> x >> y) {
			result.push_back(x);
			result.push_back(y);
		}
		return result;
	}



	

	/// reads a list of superellipses from a text file in ronin format and returns the content as a vector of superellipse-vectors
	/**
	*
	*/

	vector<vector<double>> loadSuperellipsesFromRoninTextfile(string input) {
		ifstream infile(input);
		string dummy;
		int dummyInt;

		// reading the first two lines (not needed here)
		infile >> dummy;
		infile >> dummy >> dummyInt;

		vector<vector<double>> superellipses;
	
		// reading superellipse parameters from textfile
		double xc, yc, theta, a, b, epsilon, dummyDouble;
		while (infile >> dummy >> dummyDouble >> xc >> yc >> dummyDouble >> dummyDouble >> dummyDouble >> dummyDouble >> a >> b >> epsilon >> theta >> dummyDouble) {
			vector<double> readSuperellipse = { xc, yc, theta, a, b, epsilon };
			superellipses.emplace_back(readSuperellipse);
		}
		return superellipses;
	}





	/// reads a list of superellipses from a text file in beztix format and returns the content as a vector of superellipse-vectors
	/**
	*
	*/

	vector<vector<double>> loadSuperellipsesFromBeztixTextfile(string input) {
		ifstream infile(input);
		string dummy;

		// reading width & height;
		int width, height;
		infile >> dummy >> width;
		infile >> dummy >> height;

		vector<vector<double>> superellipses;

		// reading superellipse parameters from textfile
		double xc, yc, theta, a, b, epsilon;
		while (infile >> xc >> yc >> theta >> a >> b >> epsilon) {
			vector<double> readSuperellipse = { xc, yc, theta, a, b, epsilon };
			superellipses.emplace_back(readSuperellipse);
		}
		return superellipses;
	}





	std::vector<std::vector<viral_core::vector>> readPositionsGridFromTextfile(string input, int dimX, int dimY) {

		ifstream infile(input);
		float xPos, yPos, zPos;

		std::vector<std::vector<viral_core::vector>> positionsGrid(480, std::vector<viral_core::vector>(640, viral_core::vector()));
		
		for (int y = 0; y < dimY; y++) {
			for (int x = 0; x < dimX; x++) {
				infile >> xPos >> yPos >> zPos;
				viral_core::vector currentVec(xPos, yPos, zPos);
				positionsGrid[y][x] = currentVec;
			}
		}


		infile.close();

		return positionsGrid;

	}




	std::vector<std::vector<viral_core::vector>> readPositionsGridFromBinaryfile(string input, int dimX, int dimY) {

		ifstream infile(input, ios::binary);
		

		std::vector<std::vector<viral_core::vector>> positionsGrid(dimY, std::vector<viral_core::vector>(dimX, viral_core::vector()));

		for (int y = 0; y < dimY; y++) {
			for (int x = 0; x < dimX; x++) {
				viral_core::vector currentVec;
				infile.read((char *)&currentVec, sizeof(viral_core::vector));
				positionsGrid[y][x] = currentVec;
			}
		}


		infile.close();

		return positionsGrid;

	}




	std::vector<int> readIntVectorFromTextfile(std::string input) {
		ifstream infile(input);
		
		std::vector<int> output;
		int in;

		while (infile >> in) {
			output.emplace_back(in);
		}

		infile.close();

		return output;
	}




	std::vector<rec::frustum> readFrustumsFromTextfile(std::string input) {
		ifstream infile(input);

		std::vector<rec::frustum> output;
		float x, y, z;
		char c;
		while (infile >> x >> c >> y >> c >> z) {
			std::vector<viral_core::vector> frustCorners;

			viral_core::vector currentCorner = viral_core::vector(x, y, z);
			frustCorners.push_back(currentCorner);


			for (int i = 0; i < 7; i++) {
				infile >> x >> c >> y >> c >> z;
				currentCorner = viral_core::vector(x, y, z);
				frustCorners.push_back(currentCorner);
			}

			rec::frustum currentFrustum(frustCorners.at(0), frustCorners.at(1), frustCorners.at(2), frustCorners.at(3),
				frustCorners.at(4), frustCorners.at(5), frustCorners.at(6), frustCorners.at(7));
			output.push_back(currentFrustum);


		}
		infile.close();

		return output;

	}




	std::vector<rec::pyramid> readPyramidsFromTextfile(std::string input) {
		ifstream infile(input);

		std::vector<rec::pyramid> output;
		float x, y, z;
		char c;
		while (infile >> x >> c >> y >> c >> z) {
			std::vector<viral_core::vector> pyramidCorners;

			viral_core::vector currentCorner = viral_core::vector(x, y, z);
			pyramidCorners.push_back(currentCorner);


			for (int i = 0; i < 4; i++) {
				infile >> x >> c >> y >> c >> z;
				currentCorner = viral_core::vector(x, y, z);
				pyramidCorners.push_back(currentCorner);
			}

			rec::pyramid currentPyramid(pyramidCorners.at(0), pyramidCorners.at(1), pyramidCorners.at(2), pyramidCorners.at(3), pyramidCorners.at(4));
			output.push_back(currentPyramid);


		}
		infile.close();

		return output;

	}



}