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


	void writePositionsGridToTextfile(std::string location, std::vector<std::vector<viral_core::vector>> positionsGrid) {

		std::ofstream outfile(location, std::ios_base::trunc);

		for (int y = 0; y < positionsGrid.size(); y++) {
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
			std::vector<viral_core::vector> currentPositionsLine = positionsGrid.at(y);
			for(int x = 0; x < currentPositionsLine.size(); x++) {
				viral_core::vector currentPosition = currentPositionsLine.at(x);
				outfile.write((char *) &currentPosition, sizeof(viral_core::vector));
			}
		}

		outfile.close();


	}


}