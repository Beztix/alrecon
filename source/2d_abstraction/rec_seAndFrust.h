#ifndef REC_SEANDFRUST_H
#define REC_SEANDFRUST_H


#include "rec_frustum.h"
#include "se_util.h"
#include <iostream>
#include <string>


namespace rec {



	// struct representing a fitted superellipse, its bounding box and the corresponding frustum

	struct seAndFrust {

		int camera;

		se::superellipse fittedEllipse;

		viral_core::vector2f corner1;
		viral_core::vector2f corner2;
		viral_core::vector2f corner3;
		viral_core::vector2f corner4;

		frustum frust;


		
		//empty default constructor
		seAndFrust() {

		}


		//constructor receiving the ellipse and creating the bounding box
		seAndFrust(int cam, se::superellipse mfittedEllipse) {
			camera = cam;
			fittedEllipse = mfittedEllipse;

			std::vector<viral_core::vector2f> corners = se::getBoundingboxOfSuperellipse(fittedEllipse.xc, fittedEllipse.yc, fittedEllipse.a, fittedEllipse.b, fittedEllipse.epsilon, fittedEllipse.theta);
			corner1 = corners.at(0);
			corner2 = corners.at(1);
			corner3 = corners.at(2);
			corner4 = corners.at(3);
		}

		//constructor receiving only the corners
		seAndFrust(int cam, viral_core::vector2f mcorner1, viral_core::vector2f mcorner2, viral_core::vector2f mcorner3, viral_core::vector2f mcorner4) {
			camera = cam;
			corner1 = mcorner1;
			corner2 = mcorner2;
			corner3 = mcorner3;
			corner4 = mcorner4;
		}




		void setFrustum(frustum f) {
			frust = f;
		}



		std::string toBBString() {
			return "[ (" + std::to_string(corner1.x) + "," + std::to_string(corner1.y) + ")(" + std::to_string(corner2.x) + "," + std::to_string(corner2.y) + ")(" +
				std::to_string(corner3.x) + "," + std::to_string(corner3.y) + ")(" + std::to_string(corner4.x) + "," + std::to_string(corner4.y) + " ]";
		}

	};
	
	std::ostream& operator<<(std::ostream& os, const rec::seAndFrust& c);


}








#endif /* REC_SEANDFRUST_H*/
