#ifndef REC_RECONSTRUCTION_FRUSTUM_H
#define REC_RECONSTRUCTION_FRUSTUM_H


#include <viral_core/geo_3d.hpp>
#include <vector>
#include <viral_core/geo_3d.hpp>

namespace rec {

	void createObject3DTree(std::vector<viral_core::vector> cameraPositions, std::vector<std::vector<std::vector<viral_core::vector>>> &directionsGrids,
		int nrOfCams, int width, int height, int offset);

}



#endif /*REC_RECONSTRUCTION_FRUSTUM_H*/
