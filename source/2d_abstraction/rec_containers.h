#ifndef REC_CONTAINERS_H
#define REC_CONTAINERS_H


#include "rec_primitive3D.h"
#include "se_util.h"
#include <iostream>
#include <string>


namespace rec {



	// struct representing a fitted superellipse, its bounding box and the corresponding frustum
	struct seAndFrust {

		int cameraIndex;
		int offSetX;
		int offSetY;

		se::superellipse fittedEllipse;

		//bounding box
		viral_core::vector2f corner1;
		viral_core::vector2f corner2;
		viral_core::vector2f corner3;
		viral_core::vector2f corner4;

		rec::frustum frust;


		
		//empty default constructor
		seAndFrust() {

		}


		//constructor receiving the ellipse and creating the bounding box (NOT setting the frustum yet)
		seAndFrust(int cam, int offSetX, int offSetY, se::superellipse fittedEllipse) {
			cameraIndex = cam;
			this->offSetX = offSetX;
			this->offSetY = offSetY;
			this->fittedEllipse = fittedEllipse;

			std::vector<viral_core::vector2f> corners = se::getBoundingboxOfSuperellipse(fittedEllipse.xc, fittedEllipse.yc, fittedEllipse.a, fittedEllipse.b, fittedEllipse.epsilon, fittedEllipse.theta);
			corner1 = corners.at(0);
			corner2 = corners.at(1);
			corner3 = corners.at(2);
			corner4 = corners.at(3);
		}


		//constructor receiving only the corners of the bounding box (NOT setting SE or frustum)
		seAndFrust(int cam, int offSetX, int offSetY, viral_core::vector2f corner1, viral_core::vector2f corner2, 
			viral_core::vector2f corner3, viral_core::vector2f corner4) {
			cameraIndex = cam;
			this->offSetX = offSetX;
			this->offSetY = offSetY;
			this->corner1 = corner1;
			this->corner2 = corner2;
			this->corner3 = corner3;
			this->corner4 = corner4;
		}


		//constructor receiving only a frustum
		seAndFrust(frustum frust) {
			this->frust = frust;
		}



		void setFrustum(frustum frust) {
			this->frust = frust;
		}



		std::string toBBString() {
			return "[ (" + std::to_string(corner1.x) + "," + std::to_string(corner1.y) + ")(" + std::to_string(corner2.x) + "," + std::to_string(corner2.y) + ")(" +
				std::to_string(corner3.x) + "," + std::to_string(corner3.y) + ")(" + std::to_string(corner4.x) + "," + std::to_string(corner4.y) + " ]";
		}

	};
	
	std::ostream& operator<<(std::ostream& os, const rec::seAndFrust& c);
















	// struct representing a fitted superellipse, its bounding box and the corresponding pyramid
	struct seAndPyramid {

		int level;
		int camInternalID;
		int totalID;
		int cameraIndex;
		int offSetX;
		int offSetY;

		se::superellipse fittedEllipse;

		//bounding box
		viral_core::vector2f corner1;
		viral_core::vector2f corner2;
		viral_core::vector2f corner3;
		viral_core::vector2f corner4;

		rec::pyramid pyr;


		void setCamInternalID(int id) {
			this->camInternalID = id;
		}

		void setLevel(int level) {
			this->level = level;
		}

		void setTotalID(int id) {
			this->totalID = id;
		}


		//empty default constructor
		seAndPyramid() {

		}


		//constructor receiving the ellipse and creating the bounding box (NOT setting the pyramid yet)
		seAndPyramid(int cam, int offSetX, int offSetY, se::superellipse fittedEllipse) {
			cameraIndex = cam;
			this->offSetX = offSetX;
			this->offSetY = offSetY;
			this->fittedEllipse = fittedEllipse;

			std::vector<viral_core::vector2f> corners = se::getBoundingboxOfSuperellipse(fittedEllipse.xc, fittedEllipse.yc, fittedEllipse.a, fittedEllipse.b, fittedEllipse.epsilon, fittedEllipse.theta);
			corner1 = corners.at(0);
			corner2 = corners.at(1);
			corner3 = corners.at(2);
			corner4 = corners.at(3);
		}


		//constructor receiving only the corners of the bounding box (NOT setting SE or pyramid)
		seAndPyramid(int cam, int offSetX, int offSetY, viral_core::vector2f corner1, viral_core::vector2f corner2,
			viral_core::vector2f corner3, viral_core::vector2f corner4) {
			cameraIndex = cam;
			this->offSetX = offSetX;
			this->offSetY = offSetY;
			this->corner1 = corner1;
			this->corner2 = corner2;
			this->corner3 = corner3;
			this->corner4 = corner4;
		}


		//constructor receiving only a pyramid
		seAndPyramid(rec::pyramid pyr) {
			this->pyr = pyr;
		}



		void setPyramid(rec::pyramid pyr) {
			this->pyr = pyr;
		}



		std::string toBBString() {
			return "[ (" + std::to_string(corner1.x) + "," + std::to_string(corner1.y) + ")(" + std::to_string(corner2.x) + "," + std::to_string(corner2.y) + ")(" +
				std::to_string(corner3.x) + "," + std::to_string(corner3.y) + ")(" + std::to_string(corner4.x) + "," + std::to_string(corner4.y) + " ]";
		}

	};

	std::ostream& operator<<(std::ostream& os, const rec::seAndPyramid& c);







	struct intersectionLookup {

		char intersection;			//specifying the intersection of the two objects - 0 = not calculated yet, 1 = intersection, -1 = no intersection
		rec::aabb intersectionBoundingBox;

		//empty default constructor
		intersectionLookup() {
			intersection = 0;
		}

		void setIntersectTrue() {
			intersection = 1;
		}

		void setIntersectFalse() {
			intersection = -1;
		}

		void setIntersectionBoundingBox(rec::aabb intersectionBoundingBox) {
			this->intersectionBoundingBox = intersectionBoundingBox;
		}


	};








}








#endif /* REC_CONTAINERS_H*/
