#ifndef REC_FRUSTUM_INTERSECTION_H
#define REC_FRUSTUM_INTERSECTION_H


#include "rec_frustum.h"
#include "rec_seAndFrust.h"
#include <vector>



namespace rec {

	bool doFrustumsIntesect(rec::frustum firstFrustum, rec::frustum secondFrustum);

	bool doFrustumsIntesect(rec::seAndFrust* firstFrustum, rec::seAndFrust* secondFrustum);

	bool doMultipleFrustumsIntersect(std::vector<rec::seAndFrust*> fList, rec::seAndFrust* b);


	std::vector<std::vector<rec::seAndFrust*>> intersectAllFrustums(std::vector<std::vector<rec::seAndFrust*>> frustumLists);


	std::vector<std::vector<rec::seAndFrust*>> testMultipleFrustumsWithMultipleFrustumsForIntersection
		(std::vector<rec::seAndFrust*> fList, std::vector<std::vector<rec::seAndFrust*>> FrustLists, int currentList);

}


#endif /*REC_FRUSTUM_INTERSECTION_H*/
