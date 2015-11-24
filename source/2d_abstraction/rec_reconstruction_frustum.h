#ifndef REC_RECONSTRUCTION_FRUSTUM_H
#define REC_RECONSTRUCTION_FRUSTUM_H


#include <vector>
#include <viral_core/geo_3d.hpp>
#include "rec_sensor.h"
#include "tree.hh"
#include "rec_containers.h"
#include "rec_object3D.h"

namespace rec {

	tree<rec::object3D> createObject3DTree(std::vector<viral_core::vector> cameraPositions, std::vector<std::vector<std::vector<viral_core::vector>>> &directionsGrids,
		std::vector<tree<rec::seAndFrust>> &seAndFrustTrees, int nrOfCams, int width, int height, int offset);

}



#endif /*REC_RECONSTRUCTION_FRUSTUM_H*/
