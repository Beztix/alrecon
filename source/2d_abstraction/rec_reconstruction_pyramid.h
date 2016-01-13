#ifndef REC_RECONSTRUCTION_PYRAMID_H
#define REC_RECONSTRUCTION_PYRAMID_H


#include <vector>
#include <viral_core/geo_vector.hpp>
#include "rec_sensor.h"
#include "tree.hh"
#include "rec_containers.h"
#include "rec_object3D.h"

namespace rec {

	tree<rec::object3D> createObject3DTree(std::vector<viral_core::vector> cameraPositions, std::vector<std::vector<std::vector<viral_core::vector>>> &directionsGrids,
		std::vector<tree<rec::seAndPyramid>> &seAndPyramidTrees, std::vector<std::vector<int>> treeContentCounts, int nrOfCams, int width, int height, int offset, rec::aabb workspace);


	std::vector<viral_core::vector> reconstruct_object3DTree(int stepsize, std::vector<rec::sensor> sensors, tree<rec::object3D> object3DTree, int level);

	std::vector<std::vector<viral_core::vector>> reconstruct_object3DTree_objectSeparated(rec::aabb workspace, int stepsize, std::vector<rec::sensor> sensors, tree<rec::object3D> object3DTree, int level);
}



#endif /*REC_RECONSTRUCTION_PYRAMID_H*/
