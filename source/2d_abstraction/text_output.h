#ifndef TEXT_OUTPUT_H
#define TEXT_OUTPUT_H

#include <vector>
#include <viral_core/geo_3d.hpp>


namespace text_output {


	void writePositionsGridToTextfile(std::string location, std::vector<std::vector<viral_core::vector>> positionsGrid);

	void writePositionsGridToBinaryfile(std::string location, std::vector<std::vector<viral_core::vector>> positionsGrid);



}


#endif /* TEXT_OUTPUT_H*/
