/**
*************************************************************************
*
* @file rec_freconstruction_frustum.cpp
*
* 
*
************************************************************************/

#include "rec_reconstruction_frustum.h"


#include <vector>


#include "rec_object3D.h"
#include "rec_seAndFrust.h"
#include "rec_frustum_intersection.h"
#include "tree.hh"


namespace rec {

	void createObject3DTree(std::vector<viral_core::vector> cameraPositions, std::vector<std::vector<std::vector<viral_core::vector>>> &directionsGrids, 
		std::vector<tree<rec::seAndFrust>> seAndFrustTrees, int nrOfCams, int width, int height, int offset) {


		tree<rec::object3D> object3DTree;
		object3DTree.clear();

		// list of all seAndFrustPLists, one per camera
		std::vector<std::vector<rec::seAndFrust*>> seAndFrustPLists;


		
		
		for (int i = 0; i < nrOfCams; i++) {
			tree<rec::seAndFrust>::iterator seAndFrustTreeTop = seAndFrustTrees.at(i).begin();

			std::vector<rec::seAndFrust> currentSeAndFrustList;
			std::vector<rec::seAndFrust*> currentSeAndFrustPList;
			
			rec::seAndFrust& rootSeAndFrust = *seAndFrustTreeTop;



			//calculate frustum
			viral_core::vector camPosition = cameraPositions.at(i);
			viral_core::vector direction1 = directionsGrids.at(i).at(rootSeAndFrust.corner1.y + offset).at(rootSeAndFrust.corner1.x + offset);
			viral_core::vector direction2 = directionsGrids.at(i).at(rootSeAndFrust.corner2.y + offset).at(rootSeAndFrust.corner2.x + offset);
			viral_core::vector direction3 = directionsGrids.at(i).at(rootSeAndFrust.corner3.y + offset).at(rootSeAndFrust.corner3.x + offset);
			viral_core::vector direction4 = directionsGrids.at(i).at(rootSeAndFrust.corner4.y + offset).at(rootSeAndFrust.corner4.x + offset);
			viral_core::vector near_top_left = camPosition + (direction2 * 500);
			viral_core::vector near_top_right = camPosition + (direction1 * 500);
			viral_core::vector near_bot_left = camPosition + (direction3 * 500);
			viral_core::vector near_bot_right = camPosition + (direction4 * 500);
			viral_core::vector far_top_left = camPosition + (direction2 * 5000);
			viral_core::vector far_top_right = camPosition + (direction1 * 5000);
			viral_core::vector far_bot_left = camPosition + (direction3 * 5000);
			viral_core::vector far_bot_right = camPosition + (direction4 * 5000);

			rec::frustum currentFrust = rec::frustum(near_bot_left, near_bot_right, near_top_right, near_top_left,
													far_bot_left, far_bot_right, far_top_right, far_top_left);

			rootSeAndFrust.setFrustum(currentFrust);


			currentSeAndFrustList.push_back(rootSeAndFrust);
		
			

			currentSeAndFrustPList.push_back(&rootSeAndFrust);
			seAndFrustPLists.push_back(currentSeAndFrustPList);
		}



		// ==== intersect initial seAndFrustPLists ==== 
		std::vector<std::vector<rec::seAndFrust*>> intersection = rec::intersectAllFrustums(seAndFrustPLists);


		//intersection of the whole camera viewing frustum should be one object
		if (intersection.size() != 1) {
			//ERROR
			return;
		}

		//add root node to the tree
		tree<rec::object3D>::iterator top = object3DTree.begin();
		rec::object3D rootObject = object3D(intersection.at(0));
		object3DTree.insert(top, rootObject);

		

		int a = 5;

	}




}