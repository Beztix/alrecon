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



	void appendFrustumToTextFile(std::string location, rec::frustum frust) {

		std::ofstream outfile(location, std::ios_base::app);

		for (int i = 0; i < 8; i++) {
			outfile << frust.points[i].x << ", " << frust.points[i].y << ", " << frust.points[i].z << "\n";
		}
		
		outfile.close();

	}




	void writeObject3DToTextFiles(std::string name, rec::object3D o) {

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