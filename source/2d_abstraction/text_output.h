#ifndef TEXT_OUTPUT_H
#define TEXT_OUTPUT_H

#include <vector>
#include <viral_core/geo_vector.hpp>

#include "rec_object3D.h"


namespace text_output {

	void writeObject3DToFrustumTextFiles(std::string name, rec::object3D o);

	void writeObject3DToPyramidTextFiles(std::string name, rec::object3D o);

	void appendFrustumToTextFile(std::string location, rec::frustum frust);

	void appendPyramidToTextFile(std::string location, rec::pyramid pyr);

	void writePositionsGridToTextfile(std::string location, std::vector<std::vector<viral_core::vector>> positionsGrid);

	void writePositionsGridToBinaryfile(std::string location, std::vector<std::vector<viral_core::vector>> positionsGrid);

	void writeIntVectorToTextfile(std::string location, std::vector<int> data);

}


#endif /* TEXT_OUTPUT_H*/
