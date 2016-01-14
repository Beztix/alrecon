#ifndef REC_PYRAMID_INTERSECTION_H
#define REC_PYRAMID_INTERSECTION_H


#include "rec_object3D.h"
#include "rec_containers.h"
#include "tree.hh"
#include <vector>



namespace rec {

	



	std::vector<rec::object3D> intersectAllPyramids(std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> PyramidLists, rec::aabb workspace, rec::intersectionLookup** lookupMatrix);


}


#endif /*REC_PYRAMID_INTERSECTION_H*/
