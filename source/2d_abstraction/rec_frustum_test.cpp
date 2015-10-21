/**
*************************************************************************
*
* @file rec_frustum_test.cpp
*
* 
*
************************************************************************/

#include "rec_frustum_test.h"
#include "rec_frustum_util.h"

#include <opencv2/core/core.hpp>


using namespace std;


namespace rec {









	//testing two frustums for intersection
	bool doFrustumsIntesect(frust firstFrustum, frust secondFrustum) {

		bool in;

		//========== test first frustum ===========

		//right plane
		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = secondFrustum.points[i] - firstFrustum.planeNormalBases[frust::x_pos];
			float dot = firstFrustum.planeNormals[frust::x_pos].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//top plane
		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = secondFrustum.points[i] - firstFrustum.planeNormalBases[frust::y_neg];
			float dot = firstFrustum.planeNormals[frust::y_neg].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//left plane
		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = secondFrustum.points[i] - firstFrustum.planeNormalBases[frust::x_neg];
			float dot = firstFrustum.planeNormals[frust::x_neg].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//bottom plane
		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = secondFrustum.points[i] - firstFrustum.planeNormalBases[frust::y_pos];
			float dot = firstFrustum.planeNormals[frust::y_pos].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//front plane
		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = secondFrustum.points[i] - firstFrustum.planeNormalBases[frust::z_near];
			float dot = firstFrustum.planeNormals[frust::z_near].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//back plane
		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = secondFrustum.points[i] - firstFrustum.planeNormalBases[frust::z_far];
			float dot = firstFrustum.planeNormals[frust::z_far].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;



		//========== test second frustum ===========

		//right plane
		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = firstFrustum.points[i] - secondFrustum.planeNormalBases[frust::x_pos];
			float dot = secondFrustum.planeNormals[frust::x_pos].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//top plane
		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = firstFrustum.points[i] - secondFrustum.planeNormalBases[frust::y_neg];
			float dot = secondFrustum.planeNormals[frust::y_neg].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//left plane
		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = firstFrustum.points[i] - secondFrustum.planeNormalBases[frust::x_neg];
			float dot = secondFrustum.planeNormals[frust::x_neg].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//bottom plane
		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = firstFrustum.points[i] - secondFrustum.planeNormalBases[frust::y_pos];
			float dot = secondFrustum.planeNormals[frust::y_pos].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//front plane
		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = firstFrustum.points[i] - secondFrustum.planeNormalBases[frust::z_near];
			float dot = secondFrustum.planeNormals[frust::z_near].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//back plane
		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = firstFrustum.points[i] - secondFrustum.planeNormalBases[frust::z_far];
			float dot = secondFrustum.planeNormals[frust::z_far].dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;


		//none of the planes separates the two frustums: there is an intersection
		return true;
	}




}