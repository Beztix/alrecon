#ifndef REC_FRUSTUM_INTERSECTION_H
#define REC_FRUSTUM_INTERSECTION_H


#include "rec_frustum.h"
#include "rec_container.h"
#include <vector>



namespace rec {

	bool doFrustumsIntesect(rec::frustum firstFrustum, rec::frustum secondFrustum);

	bool doFrustumsIntesect(rec::container* firstFrustum, rec::container* secondFrustum);

	bool doMultipleFrustumsIntersect(std::vector<rec::container*> fList, rec::container* b);


	std::vector<std::vector<rec::container*>> intersectAllFrustums(std::vector<std::vector<rec::container*>> frustumLists);


	std::vector<std::vector<rec::container*>> testMultipleFrustumsWithMultipleFrustumsForIntersection
		(std::vector<rec::container*> fList, std::vector<std::vector<rec::container*>> FrustLists, int currentList);

}


#endif /*REC_FRUSTUM_INTERSECTION_H*/
