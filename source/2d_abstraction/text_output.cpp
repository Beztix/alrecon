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



namespace text_output {


	void writePositionsGridToTextfile(std::vector<std::vector<viral_core::vector>> positionsGrid, int index) {

		std::ofstream outfile("sampledPositions_cam" + std::to_string(index) + ".txt", std::ios_base::trunc);

		for (int y = 0; y < positionsGrid.size(); y++) {
			std::vector<viral_core::vector> currentPositionsLine = positionsGrid.at(y);
			for (int x = 0; x < currentPositionsLine.size(); x++) {
				viral_core::vector currentPosition = currentPositionsLine.at(x);
				outfile << currentPosition.x << " " << currentPosition.y << " " << currentPosition.z << "\n";
			}
		}
		

		outfile.close();

	}



}