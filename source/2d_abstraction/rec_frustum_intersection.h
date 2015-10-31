#ifndef REC_FRUSTUM_INTERSECTION_H
#define REC_FRUSTUM_INTERSECTION_H


#include "rec_frustum.h"
#include <vector>



namespace rec {

	bool doFrustumsIntesect(frustum firstFrustum, frustum secondFrustum);

	bool doMultipleFrustumsIntersect(std::vector<rec::frustum> fList, rec::frustum b);


	std::vector<std::vector<rec::frustum>> intersectAllFrustums(std::vector<std::vector<rec::frustum>> frustumLists);


	std::vector<std::vector<rec::frustum>> testMultipleFrustumsWithMultipleFrustumsForIntersection
		(std::vector<rec::frustum> fList, std::vector<std::vector<rec::frustum>> FrustLists, int currentList);

}


#endif /*REC_FRUSTUM_INTERSECTION_H*/
