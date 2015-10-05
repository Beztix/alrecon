/**
*************************************************************************
*
* @file frustum_test.cpp
*
* 
*
************************************************************************/

#include "frustum_test.h"


#include "frustum_util.h"

#include <opencv2/core/core.hpp>


using namespace std;


namespace frustum{










	bool doFrustumsIntesect(frust firstFrustum, frust secondFrustum) {


		cv::Point3f vectorOne;
		cv::Point3f vectorTwo;
		cv::Point3f normal;		//always pointing inside the frustum!
		cv::Point3f normalBase;
		bool in;


		//========== test first frustum ===========

		//right plane
		vectorOne = firstFrustum.points[frust::far_xpos_ypos] - firstFrustum.points[frust::near_xpos_ypos];
		vectorTwo = firstFrustum.points[frust::near_xpos_yneg] - firstFrustum.points[frust::near_xpos_ypos];
		normal = vectorOne.cross(vectorTwo);
		normalBase = firstFrustum.points[frust::near_xpos_ypos];

		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = secondFrustum.points[i] -normalBase;
			float dot = normal.dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//top plane
		vectorOne = firstFrustum.points[frust::near_xneg_yneg] - firstFrustum.points[frust::near_xpos_yneg];
		vectorTwo = firstFrustum.points[frust::far_xpos_yneg] - firstFrustum.points[frust::near_xpos_yneg];
		normal = vectorOne.cross(vectorTwo);
		normalBase = firstFrustum.points[frust::near_xpos_yneg];

		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = secondFrustum.points[i] - normalBase;
			float dot = normal.dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//left plane
		vectorOne = firstFrustum.points[frust::near_xneg_ypos] - firstFrustum.points[frust::near_xneg_yneg];
		vectorTwo = firstFrustum.points[frust::far_xneg_yneg] - firstFrustum.points[frust::near_xneg_yneg];
		normal = vectorOne.cross(vectorTwo);
		normalBase = firstFrustum.points[frust::near_xneg_yneg];

		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = secondFrustum.points[i] - normalBase;
			float dot = normal.dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//bottom plane
		vectorOne = firstFrustum.points[frust::near_xpos_ypos] - firstFrustum.points[frust::near_xneg_ypos];
		vectorTwo = firstFrustum.points[frust::far_xneg_ypos] - firstFrustum.points[frust::near_xneg_ypos];
		normal = vectorOne.cross(vectorTwo);
		normalBase = firstFrustum.points[frust::near_xneg_ypos];

		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = secondFrustum.points[i] - normalBase;
			float dot = normal.dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//front plane
		vectorOne = firstFrustum.points[frust::near_xneg_ypos] - firstFrustum.points[frust::near_xpos_ypos];
		vectorTwo = firstFrustum.points[frust::near_xpos_yneg] - firstFrustum.points[frust::near_xpos_ypos];
		normal = vectorOne.cross(vectorTwo);
		normalBase = firstFrustum.points[frust::near_xpos_ypos];

		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = secondFrustum.points[i] - normalBase;
			float dot = normal.dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//back plane
		vectorOne = firstFrustum.points[frust::far_xneg_yneg] - firstFrustum.points[frust::far_xpos_yneg];
		vectorTwo = firstFrustum.points[frust::far_xpos_ypos] - firstFrustum.points[frust::far_xpos_yneg];
		normal = vectorOne.cross(vectorTwo);
		normalBase = firstFrustum.points[frust::far_xpos_yneg];

		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = secondFrustum.points[i] - normalBase;
			float dot = normal.dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;






		//========== test second frustum ===========

		//right plane
		vectorOne = secondFrustum.points[frust::far_xpos_ypos] - secondFrustum.points[frust::near_xpos_ypos];
		vectorTwo = secondFrustum.points[frust::near_xpos_yneg] - secondFrustum.points[frust::near_xpos_ypos];
		normal = vectorOne.cross(vectorTwo);
		normalBase = secondFrustum.points[frust::near_xpos_ypos];

		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = firstFrustum.points[i] - normalBase;
			float dot = normal.dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//top plane
		vectorOne = secondFrustum.points[frust::near_xneg_yneg] - secondFrustum.points[frust::near_xpos_yneg];
		vectorTwo = secondFrustum.points[frust::far_xpos_yneg] - secondFrustum.points[frust::near_xpos_yneg];
		normal = vectorOne.cross(vectorTwo);
		normalBase = secondFrustum.points[frust::near_xpos_yneg];

		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = firstFrustum.points[i] - normalBase;
			float dot = normal.dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//left plane
		vectorOne = secondFrustum.points[frust::near_xneg_ypos] - secondFrustum.points[frust::near_xneg_yneg];
		vectorTwo = secondFrustum.points[frust::far_xneg_yneg] - secondFrustum.points[frust::near_xneg_yneg];
		normal = vectorOne.cross(vectorTwo);
		normalBase = secondFrustum.points[frust::near_xneg_yneg];

		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = firstFrustum.points[i] - normalBase;
			float dot = normal.dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//bottom plane
		vectorOne = secondFrustum.points[frust::near_xpos_ypos] - secondFrustum.points[frust::near_xneg_ypos];
		vectorTwo = secondFrustum.points[frust::far_xneg_ypos] - secondFrustum.points[frust::near_xneg_ypos];
		normal = vectorOne.cross(vectorTwo);
		normalBase = secondFrustum.points[frust::near_xneg_ypos];

		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = firstFrustum.points[i] - normalBase;
			float dot = normal.dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//front plane
		vectorOne = secondFrustum.points[frust::near_xneg_ypos] - secondFrustum.points[frust::near_xpos_ypos];
		vectorTwo = secondFrustum.points[frust::near_xpos_yneg] - secondFrustum.points[frust::near_xpos_ypos];
		normal = vectorOne.cross(vectorTwo);
		normalBase = secondFrustum.points[frust::near_xpos_ypos];

		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = firstFrustum.points[i] - normalBase;
			float dot = normal.dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;

		//back plane
		vectorOne = secondFrustum.points[frust::far_xneg_yneg] - secondFrustum.points[frust::far_xpos_yneg];
		vectorTwo = secondFrustum.points[frust::far_xpos_ypos] - secondFrustum.points[frust::far_xpos_yneg];
		normal = vectorOne.cross(vectorTwo);
		normalBase = secondFrustum.points[frust::far_xpos_yneg];

		in = false;
		for (int i = 0; i < 8; i++) {
			cv::Point3f testVector = firstFrustum.points[i] - normalBase;
			float dot = normal.dot(testVector);
			if (dot >= 0) in = true;
		}
		if (!in) return false;



		//none of the planes separates the two frustums: there is an intersection
		return true;
	}




}