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








}