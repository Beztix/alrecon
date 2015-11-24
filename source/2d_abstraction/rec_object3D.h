#ifndef REC_OBJECT3D_H
#define REC_OBJECT3D_H



#include "rec_containers.h"
#include "tree.hh"
#include <viral_core/geo_3d.hpp>
#include <vector>


namespace rec {



	// a struct representing a 3D object in world space, defined ba a number of half-spaces
	struct object3D {

		std::vector<viral_core::vector> planeNormals;		//the plane normals of the planes defining the object, always pointing inside the object
		std::vector<viral_core::vector> planeNormalBases;	//the base vectors of the plane normals
		std::vector<rec::seAndFrust*> generatingFrustums;	//the frustums which are intersected to generate this object3D
		std::vector<rec::seAndPyramid*> generatingPyramids;	//the pyramids which are intersected to generate this object3D

		std::vector<tree<rec::seAndFrust>::pre_order_iterator> generatingFrustumsIT;
		std::vector<tree<rec::seAndPyramid>::pre_order_iterator> generatingPyramidsIT;


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
		object3D(std::vector<rec::seAndFrust*> generatingFrustums)
		{
			this->generatingFrustums = generatingFrustums;
			collectGeneratingPlanesFromFrustums();
		}

		//constructor receiving the generating pyramids (packed in seAndPyramids)
		object3D(std::vector<rec::seAndPyramid*> generatingPyramids)
		{
			this->generatingPyramids = generatingPyramids;
			collectGeneratingPlanesFromPyramids();
		}


		//constructor receiving the generating frustums (packed in seAndFrusts) as tree iterators
		object3D(std::vector<tree<rec::seAndFrust>::pre_order_iterator> generatingFrustums)
		{
			this->generatingFrustumsIT = generatingFrustums;
			for (auto n : generatingFrustums)
				this->generatingFrustums.push_back(&(*n));

			collectGeneratingPlanesFromFrustums();
		}

		//constructor receiving the generating pyramids (packed in seAndPyramids) as tree iterators
		object3D(std::vector<tree<rec::seAndPyramid>::pre_order_iterator> generatingPyramids)
		{
			this->generatingPyramidsIT = generatingPyramids;
			for (auto n : generatingPyramids)
				this->generatingPyramids.push_back(&(*n));

			collectGeneratingPlanesFromPyramids();
		}



		void collectGeneratingPlanesFromFrustums() {
			for (int i = 0; i < generatingFrustums.size(); i++) {
				rec::seAndFrust* currentseAndFrust = generatingFrustums.at(i);
				for (int k = 0; k < 6; k++) {
					viral_core::vector currentPlaneNormal = (*currentseAndFrust).frust.planeNormals[k];
					planeNormals.push_back(currentPlaneNormal);

					viral_core::vector currentPlaneNormalBase = (*currentseAndFrust).frust.planeNormalBases[k];
					planeNormalBases.push_back(currentPlaneNormalBase);
				}
			}
		}


		void collectGeneratingPlanesFromPyramids() {
			for (int i = 0; i < generatingPyramids.size(); i++) {
				rec::seAndPyramid* currentSeAndPyramid = generatingPyramids.at(i);
				for (int k = 0; k < 4; k++) {
					viral_core::vector currentPlaneNormal = (*currentSeAndPyramid).pyr.planeNormals[k];
					planeNormals.push_back(currentPlaneNormal);

					viral_core::vector currentPlaneNormalBase = (*currentSeAndPyramid).pyr.corners[k];
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
