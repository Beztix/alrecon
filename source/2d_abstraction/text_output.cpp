/**
*************************************************************************
*
* @file text_output.cpp
*
* 
*
************************************************************************/

#include "text_output.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>



namespace text_output {

	void writeVectorListToTextFile(std::string name, std::vector<viral_core::vector> vectors) {

		std::ofstream outfile(name, std::ios_base::app);

		for (viral_core::vector v : vectors) {
			outfile << v.x << " " << v.y << " " << v.z << "\n";
		}
		outfile.close();

	}




	void appendFrustumToTextFile(std::string location, rec::frustum frust) {

		std::ofstream outfile(location, std::ios_base::app);

		for (int i = 0; i < 8; i++) {
			outfile << frust.points[i].x << ", " << frust.points[i].y << ", " << frust.points[i].z << "\n";
		}
		
		outfile.close();

	}



	void appendPyramidToTextFile(std::string location, rec::pyramid pyr) {

		std::ofstream outfile(location, std::ios_base::app);

		for (int i = 0; i < 5; i++) {
			outfile << pyr.corners[i].x << ", " << pyr.corners[i].y << ", " << pyr.corners[i].z << "\n";
		}

		outfile.close();

	}




	void writeObject3DToFrustumTextFiles(std::string name, rec::object3D o) {

		std::vector<tree<rec::seAndFrust>::pre_order_iterator> generatingFrustumsIT = o.generatingFrustumsIT;

		for (int k = 0; k < generatingFrustumsIT.size(); k++) {
			tree<rec::seAndFrust>::pre_order_iterator seFrustIT = generatingFrustumsIT.at(k);
			rec::seAndFrust seFrust = *seFrustIT;
			rec::frustum frust = seFrust.frust;

			std::ofstream outfile(name + std::to_string(k) + ".txt", std::ios_base::app);

			for (int i = 0; i < 8; i++) {
				outfile << frust.points[i].x << ", " << frust.points[i].y << ", " << frust.points[i].z << "\n";
			}

			outfile.close();

		}



	}




	void writeObject3DToPyramidTextFiles(std::string name, rec::object3D o) {

		std::vector<tree<rec::seAndPyramid>::pre_order_iterator> generatingPyramidsIT = o.generatingPyramidsIT;

		for (int k = 0; k < generatingPyramidsIT.size(); k++) {
			tree<rec::seAndPyramid>::pre_order_iterator sePyrIT = generatingPyramidsIT.at(k);
			rec::seAndPyramid sePyr = *sePyrIT;
			rec::pyramid pyr = sePyr.pyr;

			std::ofstream outfile(name + std::to_string(k) + ".txt", std::ios_base::app);

			for (int i = 0; i < 5; i++) {
				outfile << pyr.corners[i].x << ", " << pyr.corners[i].y << ", " << pyr.corners[i].z << "\n";
			}

			outfile.close();

		}



	}



	void writeObject3DToTextFile(std::string name, rec::object3D o) {

		std::ofstream outfile(name, std::ios_base::app);

		for (int i = 0; i < o.planeNormals.size(); i++) {
			outfile << o.planeNormals[i].x << ", " << o.planeNormals[i].y << ", " << o.planeNormals[i].z << "\n";
		}

		outfile.close();
	}




	void writePositionsGridToTextfile(std::string location, std::vector<std::vector<viral_core::vector>> positionsGrid) {

		std::ofstream outfile(location, std::ios_base::trunc);

		for (int y = 0; y < positionsGrid.size(); y++) {
			if (y % 100 == 0) {
				std::cout << "writing row " + std::to_string(y) << std::endl;
			}
			std::vector<viral_core::vector> currentPositionsLine = positionsGrid.at(y);
			for (int x = 0; x < currentPositionsLine.size(); x++) {
				viral_core::vector currentPosition = currentPositionsLine.at(x);
				outfile << currentPosition.x << " " << currentPosition.y << " " << currentPosition.z << "\n";
			}
		}
		

		outfile.close();

	}




	void writePositionsGridToBinaryfile(std::string location, std::vector<std::vector<viral_core::vector>> positionsGrid) {

	
		std::ofstream outfile(location, std::ios::out | std::ofstream::binary);
		for (int y = 0; y < positionsGrid.size(); y++) {
			if (y % 100 == 0) {
				std::cout << "writing row " + std::to_string(y) << std::endl;
			}
			std::vector<viral_core::vector> currentPositionsLine = positionsGrid.at(y);
			for(int x = 0; x < currentPositionsLine.size(); x++) {
				viral_core::vector currentPosition = currentPositionsLine.at(x);
				outfile.write((char *) &currentPosition, sizeof(viral_core::vector));
			}
		}

		outfile.close();


	}



	void writeIntVectorToTextfile(std::string location, std::vector<int> data) {
		std::ofstream outfile(location, std::ios_base::trunc);

		for (int i = 0; i < data.size(); i++) {
			outfile << std::to_string(data.at(i)) << "\n";
		}


		outfile.close();
	}


}