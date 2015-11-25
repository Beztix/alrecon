/**
*************************************************************************
*
* @file rec_pyramid_intersection.cpp
*
* 
*
************************************************************************/

#include "rec_pyramid_intersection.h"
#include "tree.hh"
#include "util.h"

#include <viral_core/geo_3d.hpp>

using namespace std;


namespace rec {




	bool testIfApexPlaneSeparatesPyramid(viral_core::vector planeNormalBase, viral_core::vector planeNormal, rec::pyramid firstPyramid, rec::pyramid secondPyramid) {

		viral_core::vector testVector;

		std::vector<float> testDotsSecondPyramid;
		testVector = secondPyramid.corners[pyramid::bot_left] - planeNormalBase;
		testDotsSecondPyramid.push_back(planeNormal.dot(testVector));
		testVector = secondPyramid.corners[pyramid::bot_right] - planeNormalBase;
		testDotsSecondPyramid.push_back(planeNormal.dot(testVector));
		testVector = secondPyramid.corners[pyramid::top_right] - planeNormalBase;
		testDotsSecondPyramid.push_back(planeNormal.dot(testVector));
		testVector = secondPyramid.corners[pyramid::top_left] - planeNormalBase;
		testDotsSecondPyramid.push_back(planeNormal.dot(testVector));

		std::vector<float> testDotsFirstPyramid;
		testVector = firstPyramid.corners[pyramid::bot_left] - planeNormalBase;
		testDotsFirstPyramid.push_back(planeNormal.dot(testVector));
		testVector = firstPyramid.corners[pyramid::bot_right] - planeNormalBase;
		testDotsFirstPyramid.push_back(planeNormal.dot(testVector));
		testVector = firstPyramid.corners[pyramid::top_right] - planeNormalBase;
		testDotsFirstPyramid.push_back(planeNormal.dot(testVector));
		testVector = firstPyramid.corners[pyramid::top_left] - planeNormalBase;
		testDotsFirstPyramid.push_back(planeNormal.dot(testVector));

		if (util::allNegativeOrZeroFloat(testDotsSecondPyramid)) {
			//all corners of second pyramid are on the negative side of the plane
			if (util::allPositiveOrZeroFloat(testDotsFirstPyramid)) {
				//the tested plane separates both pyramids 
				return true;
			}
		}

		if (util::allPositiveOrZeroFloat(testDotsSecondPyramid)) {
			//all corners of the second pyramid are on the positive side of the plane
			if (util::allNegativeOrZeroFloat(testDotsFirstPyramid)) {
				//the tested plane separates both pyramids
				return true;
			}
		}

		return false;
	}






	//testing two pyramids for intersection
	bool doPyramidsIntersect(rec::pyramid firstPyramid, rec::pyramid secondPyramid) {


		//testing pyramids for intersection by trying to find a separating plane between the pyramids


		bool pointInside;

		//=============================================================
		//==========      test planes of first pyramid      ===========
		//=============================================================

		//bottom plane
		pointInside = false;
		for (int i = 0; i < 5; i++) {
			viral_core::vector testVector = secondPyramid.corners[i] - firstPyramid.corners[pyramid::bot];
			float dot = firstPyramid.planeNormals[pyramid::bot].dot(testVector);
			if (dot >= 0) pointInside = true;
		}
		if (!pointInside) {
			//no point of the second pyramid is on the same side of the plane as the first pyramid
			//-> the plane separates the pyramids -> no intersection 
			return false;
		}

		//right plane
		pointInside = false;
		for (int i = 0; i < 5; i++) {
			viral_core::vector testVector = secondPyramid.corners[i] - firstPyramid.corners[pyramid::right];
			float dot = firstPyramid.planeNormals[pyramid::right].dot(testVector);
			if (dot >= 0) pointInside = true;
		}
		if (!pointInside) {
			//no point of the second pyramid is on the same side of the plane as the first pyramid
			//-> the plane separates the pyramids -> no intersection 
			return false;		
		}

		//top plane
		pointInside = false;
		for (int i = 0; i < 5; i++) {
			viral_core::vector testVector = secondPyramid.corners[i] - firstPyramid.corners[pyramid::top];
			float dot = firstPyramid.planeNormals[pyramid::top].dot(testVector);
			if (dot >= 0) pointInside = true;
		}
		if (!pointInside) {
			//no point of the second pyramid is on the same side of the plane as the first pyramid
			//-> the plane separates the pyramids -> no intersection 
			return false;
		}

		//left plane
		pointInside = false;
		for (int i = 0; i < 5; i++) {
			viral_core::vector testVector = secondPyramid.corners[i] - firstPyramid.corners[pyramid::left];
			float dot = firstPyramid.planeNormals[pyramid::left].dot(testVector);
			if (dot >= 0) pointInside = true;
		}
		if (!pointInside) {
			//no point of the second pyramid is on the same side of the plane as the first pyramid
			//-> the plane separates the pyramids -> no intersection 
			return false;
		}

	
		


		//==============================================================
		//==========      test planes of second pyramid      ===========
		//==============================================================

		//bottom plane
		pointInside = false;
		for (int i = 0; i < 5; i++) {
			viral_core::vector testVector = firstPyramid.corners[i] - secondPyramid.corners[pyramid::bot];
			float dot = secondPyramid.planeNormals[pyramid::bot].dot(testVector);
			if (dot >= 0) pointInside = true;
		}
		if (!pointInside) {
			//no point of the second pyramid is on the same side of the plane as the first pyramid
			//-> the plane separates the pyramids -> no intersection 
			return false;
		}

		//right plane
		pointInside = false;
		for (int i = 0; i < 5; i++) {
			viral_core::vector testVector = firstPyramid.corners[i] - secondPyramid.corners[pyramid::right];
			float dot = secondPyramid.planeNormals[pyramid::right].dot(testVector);
			if (dot >= 0) pointInside = true;
		}
		if (!pointInside) {
			//no point of the second pyramid is on the same side of the plane as the first pyramid
			//-> the plane separates the pyramids -> no intersection 
			return false;
		}

		//top plane
		pointInside = false;
		for (int i = 0; i < 5; i++) {
			viral_core::vector testVector = firstPyramid.corners[i] - secondPyramid.corners[pyramid::top];
			float dot = secondPyramid.planeNormals[pyramid::top].dot(testVector);
			if (dot >= 0) pointInside = true;
		}
		if (!pointInside) {
			//no point of the second pyramid is on the same side of the plane as the first pyramid
			//-> the plane separates the pyramids -> no intersection 
			return false;
		}

		//left plane
		pointInside = false;
		for (int i = 0; i < 5; i++) {
			viral_core::vector testVector = firstPyramid.corners[i] - secondPyramid.corners[pyramid::left];
			float dot = secondPyramid.planeNormals[pyramid::left].dot(testVector);
			if (dot >= 0) pointInside = true;
		}
		if (!pointInside) {
			//no point of the second pyramid is on the same side of the plane as the first pyramid
			//-> the plane separates the pyramids -> no intersection 
			return false;
		}





		//=====================================================================================
		//==========      test planes connected to the apexes of both pyramids      ===========
		//=====================================================================================


		viral_core::vector planeNormalBase = secondPyramid.corners[pyramid::apex];

		viral_core::vector vectorOne = firstPyramid.corners[pyramid::apex] - planeNormalBase;
		viral_core::vector vectorTwo, planeNormal;


		//========== test planes connected to the base corners of the first pyramid ===========

		// bot_left
		vectorTwo = firstPyramid.corners[pyramid::bot_left] - planeNormalBase;
		planeNormal = vectorOne.cross(vectorTwo).normalized();

		if (testIfApexPlaneSeparatesPyramid(planeNormalBase, planeNormal, firstPyramid, secondPyramid)) {
			return false;
		}

		// bot_right
		vectorTwo = firstPyramid.corners[pyramid::bot_right] - planeNormalBase;
		planeNormal = vectorOne.cross(vectorTwo).normalized();

		if (testIfApexPlaneSeparatesPyramid(planeNormalBase, planeNormal, firstPyramid, secondPyramid)) {
			return false;
		}

		// top_right
		vectorTwo = firstPyramid.corners[pyramid::top_right] - planeNormalBase;
		planeNormal = vectorOne.cross(vectorTwo).normalized();

		if (testIfApexPlaneSeparatesPyramid(planeNormalBase, planeNormal, firstPyramid, secondPyramid)) {
			return false;
		}

		// top_left
		vectorTwo = firstPyramid.corners[pyramid::top_left] - planeNormalBase;
		planeNormal = vectorOne.cross(vectorTwo).normalized();

		if (testIfApexPlaneSeparatesPyramid(planeNormalBase, planeNormal, firstPyramid, secondPyramid)) {
			return false;
		}
		


		//========== test planes connected to the base corners of the second pyramid ===========

		// bot_left
		vectorTwo = secondPyramid.corners[pyramid::bot_left] - planeNormalBase;
		planeNormal = vectorOne.cross(vectorTwo).normalized();

		if (testIfApexPlaneSeparatesPyramid(planeNormalBase, planeNormal, firstPyramid, secondPyramid)) {
			return false;
		}

		// bot_right
		vectorTwo = secondPyramid.corners[pyramid::bot_right] - planeNormalBase;
		planeNormal = vectorOne.cross(vectorTwo).normalized();

		if (testIfApexPlaneSeparatesPyramid(planeNormalBase, planeNormal, firstPyramid, secondPyramid)) {
			return false;
		}

		// top_right
		vectorTwo = secondPyramid.corners[pyramid::top_right] - planeNormalBase;
		planeNormal = vectorOne.cross(vectorTwo).normalized();

		if (testIfApexPlaneSeparatesPyramid(planeNormalBase, planeNormal, firstPyramid, secondPyramid)) {
			return false;
		}

		// top_left
		vectorTwo = secondPyramid.corners[pyramid::top_left] - planeNormalBase;
		planeNormal = vectorOne.cross(vectorTwo).normalized();

		if (testIfApexPlaneSeparatesPyramid(planeNormalBase, planeNormal, firstPyramid, secondPyramid)) {
			return false;
		}






		//none of the tested planes separates the two pyramids: there is an intersection	
		return true;
	}










	// TODO: LOOKUP
	bool doPyramidsIntersect(tree<rec::seAndPyramid>::pre_order_iterator firstPyramid, tree<rec::seAndPyramid>::pre_order_iterator secondPyramid) {
		rec::pyramid firstPyr = (*firstPyramid).pyr;
		rec::pyramid secondPyr = (*secondPyramid).pyr;
		return doPyramidsIntersect(firstPyr, secondPyr);
	}






	/**
	*	Tests if all pyramids in pyramidOneList intersect pyramid Two.
	*	If each pyramidOne intersects pyramidTwo it returns true, otherwise false.
	*/
	bool doMultiplePyramidsIntersect(std::vector<tree<rec::seAndPyramid>::pre_order_iterator> pyramidOneList, tree<rec::seAndPyramid>::pre_order_iterator pyramidTwo) {

		for (int i = 0; i < pyramidOneList.size(); i++) {
			tree<rec::seAndPyramid>::pre_order_iterator currentPyramidOne = pyramidOneList.at(i);
			if (!doPyramidsIntersect(currentPyramidOne, pyramidTwo)) {
				return false;
			}
		}
		return true;
	}
















	/**
	*	Tests pyramids of n different cameras for intersection, n is the size of cameraPyramidLists.
	*	cameraPyramidLists contains n cameraPyramidLists, each containing an amout of pyramids which is larger or equal to 1.
	*
	*	The recursive function testMultiplePyramidsWithMultiplePyramidsForIntersection is used to test for n pyramids of the n different cameraPyramidLists if they have a common intersection.
	*
	*
	*	-cameraPyramidLists: A List of n cameraPyramidLists, with each cameraPyramidList containing the pyramids of the corresponding camera
	*	-returns:			 A List of x resulting combinations, each combination contains of n pyramids with each pyramid from one of the n cameras, while all have a common intersection.
	*/

	std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> intersectAllPyramids(std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> cameraPyramidLists) {

		std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> resultCombinations;

		//begin recursive computation with the pyramidList of camera 0
		std::vector<tree<rec::seAndPyramid>::pre_order_iterator> camera0PyramidList = cameraPyramidLists.at(0);

		for (int i = 0; i < camera0PyramidList.size(); i++) {
			tree<rec::seAndPyramid>::pre_order_iterator currentPyramid = camera0PyramidList.at(i);

			//fList contains the pyramids of the cameras already gathered and tested for intersection 
			std::vector<tree<rec::seAndPyramid>::pre_order_iterator> fList;
			fList.push_back(currentPyramid);

			//start recursive computation
			std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> resultCombinationsPart = testMultiplePyramidsWithMultiplePyramidsForIntersection(fList, cameraPyramidLists, 1);

			//append received part of the result to the whole result
			resultCombinations.insert(resultCombinations.end(), resultCombinationsPart.begin(), resultCombinationsPart.end());
		}

		return resultCombinations;
	}



























	/**
	*	Recursive function to intersect the pyramids of n different cameras.
	*	The intersecting pyramids of the k-1 cameras already processed are given by fList, this function processes camera k
	*	by testing for each currentPyramid of camera k if all of the k-1 pyramids in fList intersect it.
	*	If yes, the currentPyramids forms a valid combination with fList.
	*	If there are more cameras to pe processed, the function calls itself recursively for the next camera to be processed.
	*
	*/

	std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> testMultiplePyramidsWithMultiplePyramidsForIntersection
		(std::vector<tree<rec::seAndPyramid>::pre_order_iterator> fList, std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> cameraPyramidLists, int currentList) {

		std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> resultCombinations;

		//get cameraPyramidList which has to be processed in this recursion step
		std::vector<tree<rec::seAndPyramid>::pre_order_iterator> currentCameraPyramidList = cameraPyramidLists.at(currentList);

		//process each pyramid in currentCameraPyramidList
		for (int i = 0; i < currentCameraPyramidList.size(); i++) {
			tree<rec::seAndPyramid>::pre_order_iterator currentPyramid = currentCameraPyramidList.at(i);

			//test if all already in fList gathered pyramids intersect the currentPyramidm
			if (doMultiplePyramidsIntersect(fList, currentPyramid)) {
				std::vector<tree<rec::seAndPyramid>::pre_order_iterator> newfList = fList;
				newfList.push_back(currentPyramid);

				//this was not the last computation to be performed, there are cameraPyramidLists not evaluated yet
				if (currentList + 1 < cameraPyramidLists.size()) {
					//perform another recursive computation on the next cameraPyramidList
					std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> resultCombinationsPart = testMultiplePyramidsWithMultiplePyramidsForIntersection(newfList, cameraPyramidLists, currentList + 1);
					//append received part of the result to the whole result
					resultCombinations.insert(resultCombinations.end(), resultCombinationsPart.begin(), resultCombinationsPart.end());
				}

				//this was the last computation to be performed
				else {
					//add the gathered combination the the whole result
					resultCombinations.push_back(newfList);
				}
			}

			//not all fList pyramids intersect currentPyramid
			// -> do nothing, continue computation with next currentPyramid
		}

		return resultCombinations;

	}



}