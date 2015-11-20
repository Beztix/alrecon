#ifndef REC_OBJECT3D_H
#define REC_OBJECT3D_H



#include "rec_seAndFrust.h"
#include <viral_core/geo_3d.hpp>
#include <vector>


namespace rec {



	// a struct representing a 3D object in world space
	struct object3D {

		std::vector<viral_core::vector> planeNormals;		//the plane normals of the planes defining the object, always pointing inside the object
		std::vector<viral_core::vector> planeNormalBases;	//the base vectors of the plane normals
		std::vector<rec::seAndFrust*> generatingFrustums;	//the frustums which are intersected to generate this object3D
		

		std::vector<tree<rec::seAndFrust>::pre_order_iterator> generatingFrustumsIT;



		object3D()
		{
		
		}


		//constructor receiving defining planeNormals and the corresponding base vectors
		object3D(std::vector<viral_core::vector> mplaneNormals, std::vector<viral_core::vector> mplaneNormalBases)
		{
			planeNormals = mplaneNormals;
			planeNormalBases = mplaneNormalBases;
		}


		//constructor receiving the generating frustums (packed in seAndFrusts)
		object3D(std::vector<rec::seAndFrust*> mgeneratingFrustums)
		{
			generatingFrustums = mgeneratingFrustums;
			collectGeneratingPlanes();
		}


		object3D(std::vector<tree<rec::seAndFrust>::pre_order_iterator> mgeneratingFrustums)
		{
			generatingFrustumsIT = mgeneratingFrustums;
			for (auto n : mgeneratingFrustums)
				generatingFrustums.push_back(&(*n));

			collectGeneratingPlanes();
		}


		//TODO: ERROR
		void collectGeneratingPlanes() {
			for (int i = 0; i < generatingFrustums.size(); i++) {
				rec::seAndFrust* currentseAndFrust = generatingFrustums.at(0);
				for (int k = 0; k < 6; k++) {
					viral_core::vector currentPlaneNormal = (*currentseAndFrust).frust.planeNormals[k];
					planeNormals.push_back(currentPlaneNormal);

					viral_core::vector currentPlaneNormalBase = (*currentseAndFrust).frust.planeNormalBases[k];
					planeNormalBases.push_back(currentPlaneNormalBase);
				}
			}
		}




		bool isPointInside(viral_core::vector p) {

			//test each defining plane
			for (int i = 0; i < planeNormals.size(); i++) {
				viral_core::vector testVector = p - planeNormalBases[i];
				float dot = planeNormals[i].dot(testVector);
				//if p is on the wrong side of the plane -> outside: return false
				if (dot < 0) return false;
			}

			//p is on the right side of each plane -> inside: return true
			return true;

		}


	};
	


}








#endif /* REC_OBJECT3D_H*/
