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
#include "text_output.h"

#include <viral_core/geo_vector.hpp>
#include <viral_core/geo_primitive.hpp>
#include <viral_core/geo_intersect.hpp>

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










	//testing two pyramids for intersection using a separating axis test
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









	//calculates an axis aligned bounding box of the intersection of two pyramids by calculating all possible corners of the intersection object and building an aabb around it
	bool computePyramidIntersectionBoundingBoxInWorkspace(rec::pyramid firstPyramid, rec::pyramid secondPyramid, rec::aabb workspace,
		bool firstCamInSecondPyramid, bool secondCamInFirstPyramid, rec::aabb &result, int currentCamera, int currentCameraPyrIndex, int currentPyrOneIndex) {

		std::vector<viral_core::vector> allIntersectionPoints;


		// ======  process edges of first pyramid  ======

		if (firstCamInSecondPyramid) {
			allIntersectionPoints.push_back(firstPyramid.corners[rec::pyramid::apex]);
		}

		//generate edges of first pyramid
		viral_core::vector firstPyramidEdgeDirections[4];
		firstPyramidEdgeDirections[0] = (firstPyramid.corners[rec::pyramid::bot_left] - firstPyramid.corners[rec::pyramid::apex]).normalized();
		firstPyramidEdgeDirections[1] = (firstPyramid.corners[rec::pyramid::bot_right] - firstPyramid.corners[rec::pyramid::apex]).normalized();
		firstPyramidEdgeDirections[2] = (firstPyramid.corners[rec::pyramid::top_right] - firstPyramid.corners[rec::pyramid::apex]).normalized();
		firstPyramidEdgeDirections[3] = (firstPyramid.corners[rec::pyramid::top_left] - firstPyramid.corners[rec::pyramid::apex]).normalized();

		//generate triangles of second pyramid
		viral_core::triangle secondPyramidTriangles[4];
		secondPyramidTriangles[0] = viral_core::triangle(secondPyramid.corners[rec::pyramid::apex], secondPyramid.corners[rec::pyramid::bot_left], secondPyramid.corners[rec::pyramid::bot_right]);
		secondPyramidTriangles[1] = viral_core::triangle(secondPyramid.corners[rec::pyramid::apex], secondPyramid.corners[rec::pyramid::bot_right], secondPyramid.corners[rec::pyramid::top_right]);
		secondPyramidTriangles[2] = viral_core::triangle(secondPyramid.corners[rec::pyramid::apex], secondPyramid.corners[rec::pyramid::top_right], secondPyramid.corners[rec::pyramid::top_left]);
		secondPyramidTriangles[3] = viral_core::triangle(secondPyramid.corners[rec::pyramid::apex], secondPyramid.corners[rec::pyramid::top_left], secondPyramid.corners[rec::pyramid::bot_left]);

		//process each ray of the first pyramid
		for (viral_core::vector currentEdgeDirection : firstPyramidEdgeDirections) {
			viral_core::line currentRay = viral_core::line(firstPyramid.corners[rec::pyramid::apex], currentEdgeDirection);
			std::vector<viral_core::vector> currentIntersectionPoints;
			viral_core::vector workspaceIntersectionPoint;
			viral_core::vector hitPoint;
			float hitDistance;
			//intersect current ray with triangles of the second pyramid
			for (viral_core::triangle currentTriangle : secondPyramidTriangles) {
				//there is an intersection point
				if (viral_core::geo_intersect::intersect(currentRay, currentTriangle, hitPoint, hitDistance)) {
					//it is on the correct site of the ray
					if (hitDistance > 0) {
						currentIntersectionPoints.push_back(hitPoint);
					}
				}
				
			}
			//intersect current ray with the workspace bounding box
			workspaceIntersectionPoint = util::calculateIntersectionPointRayInAABBwithAABB(firstPyramid.corners[rec::pyramid::apex], currentEdgeDirection, workspace);
			currentIntersectionPoints.push_back(workspaceIntersectionPoint);

			//order the current intersection points along the ray
			currentIntersectionPoints = util::orderPointsByDistance(firstPyramid.corners[rec::pyramid::apex], currentIntersectionPoints);

			bool inside = firstCamInSecondPyramid;

			//process the intersection points in computed order
			for (viral_core::vector currentIntersectionPoint : currentIntersectionPoints) {

				//intersection point intersects workspace bounding box
				if (currentIntersectionPoint.nearly_equal(workspaceIntersectionPoint)) {
					if (inside) {
						allIntersectionPoints.push_back(currentIntersectionPoint);
						break;
					}
					else {
						break;
					}
				}
				//intersection point intersects other pyramid
				else {
					if (inside) {
						allIntersectionPoints.push_back(currentIntersectionPoint);
						inside = false;
					}
					else {
						allIntersectionPoints.push_back(currentIntersectionPoint);
						inside = true;
					}
				}
			}
		} 




		// ======  process edges of second pyramid  ======

		if (secondCamInFirstPyramid) {
			allIntersectionPoints.push_back(secondPyramid.corners[rec::pyramid::apex]);
		}

		//generate edges of second pyramid
		viral_core::vector secondPyramidEdgeDirections[4];
		secondPyramidEdgeDirections[0] = (secondPyramid.corners[rec::pyramid::bot_left] - secondPyramid.corners[rec::pyramid::apex]).normalized();
		secondPyramidEdgeDirections[1] = (secondPyramid.corners[rec::pyramid::bot_right] - secondPyramid.corners[rec::pyramid::apex]).normalized();
		secondPyramidEdgeDirections[2] = (secondPyramid.corners[rec::pyramid::top_right] - secondPyramid.corners[rec::pyramid::apex]).normalized();
		secondPyramidEdgeDirections[3] = (secondPyramid.corners[rec::pyramid::top_left] - secondPyramid.corners[rec::pyramid::apex]).normalized();

		//generate triangles of first pyramid
		viral_core::triangle firstPyramidTriangles[4];
		firstPyramidTriangles[0] = viral_core::triangle(firstPyramid.corners[rec::pyramid::apex], firstPyramid.corners[rec::pyramid::bot_left], firstPyramid.corners[rec::pyramid::bot_right]);
		firstPyramidTriangles[1] = viral_core::triangle(firstPyramid.corners[rec::pyramid::apex], firstPyramid.corners[rec::pyramid::bot_right], firstPyramid.corners[rec::pyramid::top_right]);
		firstPyramidTriangles[2] = viral_core::triangle(firstPyramid.corners[rec::pyramid::apex], firstPyramid.corners[rec::pyramid::top_right], firstPyramid.corners[rec::pyramid::top_left]);
		firstPyramidTriangles[3] = viral_core::triangle(firstPyramid.corners[rec::pyramid::apex], firstPyramid.corners[rec::pyramid::top_left], firstPyramid.corners[rec::pyramid::bot_left]);

		//process each ray of the second pyramid
		for (viral_core::vector currentEdgeDirection : secondPyramidEdgeDirections) {
			viral_core::line currentRay = viral_core::line(secondPyramid.corners[rec::pyramid::apex], currentEdgeDirection);
			std::vector<viral_core::vector> currentIntersectionPoints;
			viral_core::vector workspaceIntersectionPoint;
			viral_core::vector hitPoint;
			float hitDistance;
			//intersect current ray with triangles of the first pyramid
			for (viral_core::triangle currentTriangle : firstPyramidTriangles) {
				//there is an intersection point
				if (viral_core::geo_intersect::intersect(currentRay, currentTriangle, hitPoint, hitDistance)) {
					//it is on the correct site of the ray
					if (hitDistance > 0) {
						currentIntersectionPoints.push_back(hitPoint);
					}
				}
				
			}
			//intersect current ray with the workspace bounding box
			workspaceIntersectionPoint = util::calculateIntersectionPointRayInAABBwithAABB(secondPyramid.corners[rec::pyramid::apex], currentEdgeDirection, workspace);
			currentIntersectionPoints.push_back(workspaceIntersectionPoint);

			//order the current intersection points along the ray
			currentIntersectionPoints = util::orderPointsByDistance(secondPyramid.corners[rec::pyramid::apex], currentIntersectionPoints);

			bool inside = secondCamInFirstPyramid;

			//process the intersection points in computed order
			for (viral_core::vector currentIntersectionPoint : currentIntersectionPoints) {

				//intersection point intersects workspace bounding box
				if (currentIntersectionPoint.nearly_equal(workspaceIntersectionPoint)) {
					if (inside) {
						allIntersectionPoints.push_back(currentIntersectionPoint);
						break;
					}
					else {
						break;
					}
				}
				//intersection point intersects other pyramid
				else {
					if (inside) {
						allIntersectionPoints.push_back(currentIntersectionPoint);
						inside = false;
					}
					else {
						allIntersectionPoints.push_back(currentIntersectionPoint);
						inside = true;
					}
				}
			}
		}

		/*
		text_output::writeVectorListToTextFile("a-" + std::to_string(currentCamera) +"_" + std::to_string(currentCameraPyrIndex) + "_" + std::to_string(currentPyrOneIndex) + "intersectPoints.txt", allIntersectionPoints);
		text_output::appendPyramidToTextFile("a-" + std::to_string(currentCamera) + "_" + std::to_string(currentCameraPyrIndex) + "_" + std::to_string(currentPyrOneIndex) + "firstPyr.txt", firstPyramid);
		text_output::appendPyramidToTextFile("a-" + std::to_string(currentCamera) + "_" + std::to_string(currentCameraPyrIndex) + "_" + std::to_string(currentPyrOneIndex) + "secondPyr.txt", secondPyramid);
		*/

		//there is some part of the intersection inside of the workspace bounding box
		if (allIntersectionPoints.size() > 0) {
			rec::aabb intersectionBoundingBox = util::createBoundingBoxOfPoints(allIntersectionPoints);
			result = intersectionBoundingBox;
			return true;
		}

		//the intersection is completely outside of the workspace bounding box
		return false;
	}









	/**
	*	Tests if all pyramids in pyramidOneList intersect pyramid Two.
	*	If each pyramidOne intersects pyramidTwo it returns true, otherwise false.
	*/
	bool doMultiplePyramidsIntersectInsideWorkspace(std::vector<tree<rec::seAndPyramid>::pre_order_iterator> pyramidOneList, tree<rec::seAndPyramid>::pre_order_iterator pyramidTwo,
		rec::aabb workspace, rec::aabb totalIntersectionBoundingBox, rec::aabb &newTotalIntersectionBoundingBox, int currentCamera, int currentCameraPyrIndex) {

		rec::pyramid pyrTwo = (*pyramidTwo).pyr;

		for (int i = 0; i < pyramidOneList.size(); i++) {
			tree<rec::seAndPyramid>::pre_order_iterator currentPyramidOne = pyramidOneList.at(i);
			rec::pyramid currentPyrOne = (*currentPyramidOne).pyr;

			int idTwo = pyrTwo.id;
			int idOne = currentPyrOne.id;


			//calculate camera containment
			bool firstCamInSecondPyramid = false;
			bool secondCamInFirstPyramid = false;
			if (pyrTwo.isPointInside(currentPyrOne.corners[rec::pyramid::apex])) {
				firstCamInSecondPyramid = true;
			}
			if (currentPyrOne.isPointInside(pyrTwo.corners[rec::pyramid::apex])) {
				secondCamInFirstPyramid = true;
			}

			//check for intersection of the two pyramids
			if (!doPyramidsIntersect(currentPyrOne, pyrTwo)) {
				//no intersection between pyramids
				return false;
			}

			//calculate intersection bounding box
			rec::aabb currentIntersectionBoundingBox;
			if (!computePyramidIntersectionBoundingBoxInWorkspace(currentPyrOne, pyrTwo, workspace, firstCamInSecondPyramid, secondCamInFirstPyramid, 
				currentIntersectionBoundingBox, currentCamera, currentCameraPyrIndex, i)) {
				//intersection bounding box is completely outside of the workspace
				return false;
			}

			//update totalIntersectionBoundingBox by intersecting it with the currentIntersectionBoundingBox
			if (!util::doAABBsIntersect(totalIntersectionBoundingBox, currentIntersectionBoundingBox)) {
				//no intersection between the intersection bounding boxes
				return false;
			}
			newTotalIntersectionBoundingBox = util::calculateAABBIntersection(totalIntersectionBoundingBox, currentIntersectionBoundingBox);
		}
		return true;
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
		(std::vector<tree<rec::seAndPyramid>::pre_order_iterator> fList, std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> cameraPyramidLists, 
			int currentCamera, rec::aabb workspace, rec::aabb &totalIntersectionBoundingBox) {

		std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> resultCombinations;

		//get cameraPyramidList which has to be processed in this recursion step
		std::vector<tree<rec::seAndPyramid>::pre_order_iterator> currentCameraPyramidList = cameraPyramidLists.at(currentCamera);

		//process each pyramid in currentCameraPyramidList
		for (int i = 0; i < currentCameraPyramidList.size(); i++) {
			tree<rec::seAndPyramid>::pre_order_iterator currentPyramid = currentCameraPyramidList.at(i);

			//test if all already in fList gathered pyramids intersect the currentPyramid
			rec::aabb newTotalIntersectionBoundingBox;
			if (doMultiplePyramidsIntersectInsideWorkspace(fList, currentPyramid, workspace, totalIntersectionBoundingBox, newTotalIntersectionBoundingBox, currentCamera, i)) {
				std::vector<tree<rec::seAndPyramid>::pre_order_iterator> newfList = fList;
				newfList.push_back(currentPyramid);

				//this was not the last computation to be performed, there are cameraPyramidLists not evaluated yet
				if (currentCamera + 1 < cameraPyramidLists.size()) {
					//perform another recursive computation on the next cameraPyramidList
					std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> resultCombinationsPart = 
						testMultiplePyramidsWithMultiplePyramidsForIntersection(newfList, cameraPyramidLists, currentCamera + 1, workspace, newTotalIntersectionBoundingBox);
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

	std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> intersectAllPyramids
		(std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> cameraPyramidLists, rec::aabb workspace) {

		std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> resultCombinations;

		//begin recursive computation with the pyramidList of camera 0
		std::vector<tree<rec::seAndPyramid>::pre_order_iterator> camera0PyramidList = cameraPyramidLists.at(0);

		for (int i = 0; i < camera0PyramidList.size(); i++) {
			tree<rec::seAndPyramid>::pre_order_iterator currentPyramid = camera0PyramidList.at(i);

			//fList contains the pyramids of the cameras already gathered and tested for intersection 
			std::vector<tree<rec::seAndPyramid>::pre_order_iterator> fList;
			fList.push_back(currentPyramid);
			rec::aabb totalIntersectionBoundingBox = rec::aabb(); //initialize with maximum size bounding box

			//start recursive computation
			std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> resultCombinationsPart =
				testMultiplePyramidsWithMultiplePyramidsForIntersection(fList, cameraPyramidLists, 1, workspace, totalIntersectionBoundingBox);

			//append received part of the result to the whole result
			resultCombinations.insert(resultCombinations.end(), resultCombinationsPart.begin(), resultCombinationsPart.end());
		}

		return resultCombinations;
	}










}