#ifndef REC_PYRAMID_INTERSECTION_H
#define REC_PYRAMID_INTERSECTION_H


#include "rec_object3D.h"
#include "rec_containers.h"
#include "tree.hh"
#include <vector>



namespace rec {

	bool doPyramidsIntersect(rec::pyramid firstPyramid, rec::pyramid secondPyramid);

	





	bool doPyramidsIntersect(tree<rec::seAndPyramid>::pre_order_iterator firstPyramid, tree<rec::seAndPyramid>::pre_order_iterator secondPyramid);

	bool doMultiplePyramidsIntersect(std::vector<tree<rec::seAndPyramid>::pre_order_iterator> fList, tree<rec::seAndPyramid>::pre_order_iterator b);


	std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> intersectAllPyramids(std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> PyramidLists);


	std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> testMultiplePyramidsWithMultiplePyramidsForIntersection
		(std::vector<tree<rec::seAndPyramid>::pre_order_iterator> fList, std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> PyramidLists, int currentList);

}


#endif /*REC_PYRAMID_INTERSECTION_H*/
