#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

#include <vector>
#include <string>
#include <viral_core/geo_3d.hpp>



namespace text_input {

	std::vector<std::vector<viral_core::vector>> readPositionsGridFromTextfile(std::string input, int dimX, int dimY);

	std::vector<std::vector<viral_core::vector>> readPositionsGridFromBinaryfile(std::string input, int dimX, int dimY);

	std::vector<int> readIntVectorFromTextfile(std::string input);
}


#endif /* TEXT_INPUT_H*/
