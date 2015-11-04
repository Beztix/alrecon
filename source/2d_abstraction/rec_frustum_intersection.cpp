/**
*************************************************************************
*
* @file rec_frustum_intersection.cpp
*
* 
*
************************************************************************/

#include "rec_frustum_intersection.h"
#include "rec_frustum.h"
#include "rec_seAndFrust.h"

#include <viral_core/geo_3d.hpp>

using namespace std;


namespace rec {









	//testing two frustums for intersection
	bool doFrustumsIntesect(frustum firstFrustum, frustum secondFrustum) {

		bool in;

		//========== test first frustum ===========

		//right plane
		in = false;
		for (int i = 0; i < 8; i++) {
			viral_core::vector testVector = secondFrustum.points[i] - firstFrustum.planeNormalBases[frustum::x_pos];
			float dot = firstFrustum.planeNormals[frustum::x_pos].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//top plane
		in = false;
		for (int i = 0; i < 8; i++) {
			viral_core::vector testVector = secondFrustum.points[i] - firstFrustum.planeNormalBases[frustum::y_neg];
			float dot = firstFrustum.planeNormals[frustum::y_neg].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//left plane
		in = false;
		for (int i = 0; i < 8; i++) {
			viral_core::vector testVector = secondFrustum.points[i] - firstFrustum.planeNormalBases[frustum::x_neg];
			float dot = firstFrustum.planeNormals[frustum::x_neg].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//bottom plane
		in = false;
		for (int i = 0; i < 8; i++) {
			viral_core::vector testVector = secondFrustum.points[i] - firstFrustum.planeNormalBases[frustum::y_pos];
			float dot = firstFrustum.planeNormals[frustum::y_pos].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//front plane
		in = false;
		for (int i = 0; i < 8; i++) {
			viral_core::vector testVector = secondFrustum.points[i] - firstFrustum.planeNormalBases[frustum::z_near];
			float dot = firstFrustum.planeNormals[frustum::z_near].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//back plane
		in = false;
		for (int i = 0; i < 8; i++) {
			viral_core::vector testVector = secondFrustum.points[i] - firstFrustum.planeNormalBases[frustum::z_far];
			float dot = firstFrustum.planeNormals[frustum::z_far].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;



		//========== test second frustum ===========

		//right plane
		in = false;
		for (int i = 0; i < 8; i++) {
			viral_core::vector testVector = firstFrustum.points[i] - secondFrustum.planeNormalBases[frustum::x_pos];
			float dot = secondFrustum.planeNormals[frustum::x_pos].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//top plane
		in = false;
		for (int i = 0; i < 8; i++) {
			viral_core::vector testVector = firstFrustum.points[i] - secondFrustum.planeNormalBases[frustum::y_neg];
			float dot = secondFrustum.planeNormals[frustum::y_neg].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//left plane
		in = false;
		for (int i = 0; i < 8; i++) {
			viral_core::vector testVector = firstFrustum.points[i] - secondFrustum.planeNormalBases[frustum::x_neg];
			float dot = secondFrustum.planeNormals[frustum::x_neg].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//bottom plane
		in = false;
		for (int i = 0; i < 8; i++) {
			viral_core::vector testVector = firstFrustum.points[i] - secondFrustum.planeNormalBases[frustum::y_pos];
			float dot = secondFrustum.planeNormals[frustum::y_pos].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//front plane
		in = false;
		for (int i = 0; i < 8; i++) {
			viral_core::vector testVector = firstFrustum.points[i] - secondFrustum.planeNormalBases[frustum::z_near];
			float dot = secondFrustum.planeNormals[frustum::z_near].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//back plane
		in = false;
		for (int i = 0; i < 8; i++) {
			viral_core::vector testVector = firstFrustum.points[i] - secondFrustum.planeNormalBases[frustum::z_far];
			float dot = secondFrustum.planeNormals[frustum::z_far].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;


		//none of the planes separates the two frustums: there is an intersection
		return true;
	}




	// TODO: LOOKUP
	bool doFrustumsIntesect(rec::seAndFrust* firstFrustum, rec::seAndFrust* secondFrustum) {
		rec::frustum firstFrust = (*firstFrustum).frust;
		rec::frustum secondFrust = (*secondFrustum).frust;
		return doFrustumsIntesect(firstFrust, secondFrust);
	}



	/**
	*	Tests if all frustums in frustumOneList intersect frustum Two.
	*	If each frustumOne intersects frustumTwo it returns true, otherwise false.
	*/
	bool doMultipleFrustumsIntersect(std::vector<rec::seAndFrust*> frustumOneList, rec::seAndFrust* frustumTwo) {

		for (int i = 0; i < frustumOneList.size(); i++) {
			rec::seAndFrust* currentFrustumOne = frustumOneList.at(i);
			if (!doFrustumsIntesect(currentFrustumOne, frustumTwo)) {
				return false;
			}
		}
		return true;
	}



	/**
	*	Tests frustums of n different cameras for intersection, n is the size of cameraFrustumLists.
	*	cameraFrustumLists contains n cameraFrustumLists, each containing an amout of frustums which is larger or equal to 1.
	*
	*	The recursive function testMultipleFrustumsWithMultipleFrustumsForIntersection is used to test for n frustums of the n different cameraFrustumLists if they have a common intersection.
	*
	*
	*	-cameraFrustumLists: A List of n cameraFrustumLists, with each cameraFrustumList containing the frustums of the corresponding camera
	*	-returns:			 A List of x resulting combinations, each combination contains of n frustums with each frustum from one of the n cameras, while all have a common intersection.
	*/

	std::vector<std::vector<rec::seAndFrust*>> intersectAllFrustums(std::vector<std::vector<rec::seAndFrust*>> cameraFrustumLists) {

		std::vector<std::vector<rec::seAndFrust*>> resultCombinations;

		//begin recursive computation with the frustumList of camera 0
		std::vector<rec::seAndFrust*> camera0FrustumList = cameraFrustumLists.at(0);

		for (int i = 0; i < camera0FrustumList.size(); i++) {
			rec::seAndFrust* currentFrustum = camera0FrustumList.at(i);

			//fList contains the frustums of the cameras already gathered and tested for intersection 
			std::vector<rec::seAndFrust*> fList;
			fList.push_back(currentFrustum);

			//start recursive computation
			std::vector<std::vector<rec::seAndFrust*>> resultCombinationsPart = testMultipleFrustumsWithMultipleFrustumsForIntersection(fList, cameraFrustumLists, 1);

			//append received part of the result to the whole result
			resultCombinations.insert(resultCombinations.end(), resultCombinationsPart.begin(), resultCombinationsPart.end());
		}

		return resultCombinations;
	}



	/**
	*	Recursive function to intersect the frustums of n different cameras.
	*	The intersecting frustums of the k-1 cameras already processed are given by fList, this function processes camera k
	*	by testing for each currentFrustum of camera k if all of the k-1 frustums in fList intersect it.
	*	If yes, the currentFrustums forms a valid combination with fList.
	*	If there are more cameras to pe processed, the function calls itself recursively for the next camera to be processed.
	*
	*/

	std::vector<std::vector<rec::seAndFrust*>> testMultipleFrustumsWithMultipleFrustumsForIntersection
		(std::vector<rec::seAndFrust*> fList, std::vector<std::vector<rec::seAndFrust*>> cameraFrustumLists, int currentList) {

		std::vector<std::vector<rec::seAndFrust*>> resultCombinations;

		//get cameraFrustumList which has to be processed in this recursion step
		std::vector<rec::seAndFrust*> currentCameraFrustumList = cameraFrustumLists.at(currentList);

		//process each frustum in currentCameraFrustumList
		for (int i = 0; i < currentCameraFrustumList.size(); i++) {
			rec::seAndFrust* currentFrustum = currentCameraFrustumList.at(i);

			//test if all already in fList gathered frustums intersect the currentFrustm
			if (doMultipleFrustumsIntersect(fList, currentFrustum)) {
				std::vector<rec::seAndFrust*> newfList = fList;
				newfList.push_back(currentFrustum);

				//this was not the last computation to be performed, there are cameraFrustumLists not evaluated yet
				if (currentList + 1 < cameraFrustumLists.size()) {
					//perform another recursive computation on the next cameraFrustumList
					std::vector<std::vector<rec::seAndFrust*>> resultCombinationsPart = testMultipleFrustumsWithMultipleFrustumsForIntersection(newfList, cameraFrustumLists, currentList+1);
					//append received part of the result to the whole result
					resultCombinations.insert(resultCombinations.end(), resultCombinationsPart.begin(), resultCombinationsPart.end());
				}

				//this was the last computation to be performed
				else {
					//add the gathered combination the the whole result
					resultCombinations.push_back(newfList);
				}
			}

			//not all fList frustums intersect currentFrustum
			// -> do nothing, continue computation with next currentFrustum
		}

		return resultCombinations;

	}



}