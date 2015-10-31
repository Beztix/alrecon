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




	bool doMultipleFrustumsIntersect(std::vector<rec::frustum> fList, rec::frustum b) {

	}




	std::vector<std::vector<rec::frustum>> intersectAllFrustums(std::vector<std::vector<rec::frustum>> frustumLists) {

	}




	std::vector<std::vector<rec::frustum>> testMultipleFrustumsWithMultipleFrustumsForIntersection
		(std::vector<rec::frustum> fList, std::vector<std::vector<rec::frustum>> FrustLists, int currentList) {


	}



}