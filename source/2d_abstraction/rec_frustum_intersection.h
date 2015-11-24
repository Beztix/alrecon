#ifndef REC_FRUSTUM_INTERSECTION_H
#define REC_FRUSTUM_INTERSECTION_H


#include "rec_object3D.h"
#include "rec_containers.h"
#include "tree.hh"
#include <vector>



namespace rec {

	bool doFrustumsIntesect(rec::frustum firstFrustum, rec::frustum secondFrustum);

	bool doFrustumsIntesect(rec::seAndFrust* firstFrustum, rec::seAndFrust* secondFrustum);

	bool doMultipleFrustumsIntersect(std::vector<rec::seAndFrust*> fList, rec::seAndFrust* b);


	std::vector<std::vector<rec::seAndFrust*>> intersectAllFrustums(std::vector<std::vector<rec::seAndFrust*>> frustumLists);


	std::vector<std::vector<rec::seAndFrust*>> testMultipleFrustumsWithMultipleFrustumsForIntersection
		(std::vector<rec::seAndFrust*> fList, std::vector<std::vector<rec::seAndFrust*>> FrustLists, int currentList);





	bool doFrustumsIntesect(tree<rec::seAndFrust>::pre_order_iterator firstFrustum, tree<rec::seAndFrust>::pre_order_iterator secondFrustum);

	bool doMultipleFrustumsIntersect(std::vector<tree<rec::seAndFrust>::pre_order_iterator> fList, tree<rec::seAndFrust>::pre_order_iterator b);


	std::vector<std::vector<tree<rec::seAndFrust>::pre_order_iterator>> intersectAllFrustums(std::vector<std::vector<tree<rec::seAndFrust>::pre_order_iterator>> frustumLists);


	std::vector<std::vector<tree<rec::seAndFrust>::pre_order_iterator>> testMultipleFrustumsWithMultipleFrustumsForIntersection
		(std::vector<tree<rec::seAndFrust>::pre_order_iterator> fList, std::vector<std::vector<tree<rec::seAndFrust>::pre_order_iterator>> FrustLists, int currentList);

}


#endif /*REC_FRUSTUM_INTERSECTION_H*/
