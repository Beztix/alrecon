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
#include <iostream>


#include "rec_object3D.h"
#include "rec_seAndFrust.h"
#include "rec_frustum_intersection.h"
#include "tree.hh"



namespace rec {

	void createObject3DTree(std::vector<viral_core::vector> cameraPositions, std::vector<std::vector<std::vector<viral_core::vector>>> &directionsGrids, 
		std::vector<tree<rec::seAndFrust>> &seAndFrustTrees, int nrOfCams, int width, int height, int offset) {

		double CAM_NEAR_PLANE = 500;
		double CAM_FAR_PLANE = 5000;


		int depthOfSeAndFrustTrees = seAndFrustTrees.at(0).max_depth();

		tree<rec::object3D> object3DTree;
		object3DTree.clear();

		// list of all seAndFrustPLists, one per camera
		// these lists contain pointers to the objects to be intersected
		std::vector<std::vector<rec::seAndFrust*>> seAndFrustPLists;


		std::vector<std::vector<tree<rec::seAndFrust>::pre_order_iterator>> seAndFrustITLists;




		//====================================
		//===    Root Nodes of 2D Trees    ===
		//====================================


		// for each camera
		for (int i = 0; i < nrOfCams; i++) {
			tree<rec::seAndFrust>::pre_order_iterator seAndFrustTreeTop = seAndFrustTrees.at(i).begin();

			// list of pointers to the current seAndFrusts
			std::vector<rec::seAndFrust*> currentSeAndFrustPList;


			std::vector<tree<rec::seAndFrust>::pre_order_iterator> currentSeAndFrustItList;
			
			// get reference to rootSeAndFrust
			rec::seAndFrust& rootSeAndFrust = *seAndFrustTreeTop;

			//calculate frustum of the root node (which represents the whole camera viewing frustum)
			viral_core::vector camPosition = cameraPositions.at(i);
			viral_core::vector direction1 = directionsGrids.at(i).at(rootSeAndFrust.corner1.y + offset).at(rootSeAndFrust.corner1.x + offset);
			viral_core::vector direction2 = directionsGrids.at(i).at(rootSeAndFrust.corner2.y + offset).at(rootSeAndFrust.corner2.x + offset);
			viral_core::vector direction3 = directionsGrids.at(i).at(rootSeAndFrust.corner3.y + offset).at(rootSeAndFrust.corner3.x + offset);
			viral_core::vector direction4 = directionsGrids.at(i).at(rootSeAndFrust.corner4.y + offset).at(rootSeAndFrust.corner4.x + offset);
			viral_core::vector near_top_left = camPosition + (direction2 * CAM_NEAR_PLANE);
			viral_core::vector near_top_right = camPosition + (direction1 * CAM_NEAR_PLANE);
			viral_core::vector near_bot_left = camPosition + (direction3 * CAM_NEAR_PLANE);
			viral_core::vector near_bot_right = camPosition + (direction4 * CAM_NEAR_PLANE);
			viral_core::vector far_top_left = camPosition + (direction2 * CAM_FAR_PLANE);
			viral_core::vector far_top_right = camPosition + (direction1 * CAM_FAR_PLANE);
			viral_core::vector far_bot_left = camPosition + (direction3 * CAM_FAR_PLANE);
			viral_core::vector far_bot_right = camPosition + (direction4 * CAM_FAR_PLANE);

			rec::frustum currentFrust = rec::frustum(near_bot_left, near_bot_right, near_top_right, near_top_left,
													far_bot_left, far_bot_right, far_top_right, far_top_left);
			rootSeAndFrust.setFrustum(currentFrust);

			// add pointer to the rootSeAndFrust to the list of current seAndFrusts
			currentSeAndFrustPList.push_back(&rootSeAndFrust);


			currentSeAndFrustItList.push_back(seAndFrustTreeTop);


			// add the list of pointers of the current seAndFrusts to the list of seAndFrustPList
			seAndFrustPLists.push_back(currentSeAndFrustPList);


			seAndFrustITLists.push_back(currentSeAndFrustItList);
		}

		

		// ###### intersect initial seAndFrustITLists ######
		std::vector<std::vector<tree<rec::seAndFrust>::pre_order_iterator>> intersectionIT = rec::intersectAllFrustums(seAndFrustITLists);
		seAndFrustITLists.clear();


		//intersection of the whole camera viewing frustum should be one object
		if (intersectionIT.size() != 1) {
			//ERROR
			//TODO: WHAT NOW?
			return;
		}

		//add root node to the tree
		rec::object3D rootObject = object3D(intersectionIT.at(0));
		object3DTree.set_head(rootObject);
		tree<rec::object3D>::iterator object3DTreeTop = object3DTree.begin();






		//================================================
		//===    Process further levels of 2D trees    ===
		//================================================



		// as long as there are unprocessed levels of the 2D trees left:
		for (int currentLevel = 0; currentLevel < depthOfSeAndFrustTrees; currentLevel++) {

			std::cout << std::endl;
			std::cout << "==== processing level " + std::to_string(currentLevel) + " ====" << std::endl;
			std::cout << std::endl;


			tree<rec::object3D>::fixed_depth_iterator currentLevel3DIterator = object3DTree.begin_fixed(object3DTreeTop, currentLevel);

			int o = 0;
			// for each object3D on the current level of the 3D tree
			while (object3DTree.is_valid(currentLevel3DIterator)) {
				
				std::cout << "==== processing object " + std::to_string(o) + " ====" << std::endl;

				// get reference to current object3D
				rec::object3D& currentObject3D = *currentLevel3DIterator;

				// process each camera = each generating frustum 
				// (generating frustums live on currentLevel of the 2D trees)
				for (int cam = 0; cam < seAndFrustTrees.size(); cam++) {

					std::cout << "processing camera " + std::to_string(cam) << std::endl;

					tree<rec::seAndFrust>& currentSeAndFrustTree = seAndFrustTrees.at(cam);

					// list containing the frustums of the current camera which should be included in the next frustum intersections step
					std::vector<tree<rec::seAndFrust>::pre_order_iterator> currentSeAndFrustITList;

					// generating frustum of the current 3D object of the current camera
					tree<rec::seAndFrust>::pre_order_iterator currentSeAndFrustIT = currentObject3D.generatingFrustumsIT.at(cam);

					// create iterator of the children of currentSeAndFrust, which are the new frustums to be tested
					// (they live on currentLevel + 1 of the 2D trees)
					tree<rec::seAndFrust>::sibling_iterator childrenITbegin = currentSeAndFrustIT.begin();
					tree<rec::seAndFrust>::sibling_iterator childrenITend = currentSeAndFrustIT.end();
					
					// add all children to the currentSeAndFrustITList
					while (childrenITbegin != childrenITend) {

						rec::seAndFrust& currentChild = *childrenITbegin;

						//calculate frustum of the current child
						viral_core::vector camPosition = cameraPositions.at(cam);
						viral_core::vector direction1 = directionsGrids.at(cam).at(currentChild.corner1.y + offset).at(currentChild.corner1.x + offset);
						viral_core::vector direction2 = directionsGrids.at(cam).at(currentChild.corner2.y + offset).at(currentChild.corner2.x + offset);
						viral_core::vector direction3 = directionsGrids.at(cam).at(currentChild.corner3.y + offset).at(currentChild.corner3.x + offset);
						viral_core::vector direction4 = directionsGrids.at(cam).at(currentChild.corner4.y + offset).at(currentChild.corner4.x + offset);
						viral_core::vector near_top_left = camPosition + (direction2 * CAM_NEAR_PLANE);
						viral_core::vector near_top_right = camPosition + (direction1 * CAM_NEAR_PLANE);
						viral_core::vector near_bot_left = camPosition + (direction3 * CAM_NEAR_PLANE);
						viral_core::vector near_bot_right = camPosition + (direction4 * CAM_NEAR_PLANE);
						viral_core::vector far_top_left = camPosition + (direction2 * CAM_FAR_PLANE);
						viral_core::vector far_top_right = camPosition + (direction1 * CAM_FAR_PLANE);
						viral_core::vector far_bot_left = camPosition + (direction3 * CAM_FAR_PLANE);
						viral_core::vector far_bot_right = camPosition + (direction4 * CAM_FAR_PLANE);

						rec::frustum currentFrust = rec::frustum(near_bot_left, near_bot_right, near_top_right, near_top_left,
							far_bot_left, far_bot_right, far_top_right, far_top_left);
						currentChild.setFrustum(currentFrust);

						currentSeAndFrustITList.push_back(tree<rec::seAndFrust>::pre_order_iterator(childrenITbegin));
						childrenITbegin++;
					}

					seAndFrustITLists.push_back(currentSeAndFrustITList);
				}


				// ###### intersect current seAndFrustITLists ######
				intersectionIT = rec::intersectAllFrustums(seAndFrustITLists);
				seAndFrustITLists.clear();

				// add all objects received from intersection as children of the currentObject3D to the object3DTree
				for (int i = 0; i < intersectionIT.size(); i++) {
					rec::object3D currentIntersectionObject = object3D(intersectionIT.at(0));
					object3DTree.append_child(currentLevel3DIterator, currentIntersectionObject);
				}
				

				// proceed to next object of the current level
				currentLevel3DIterator++;
				o++;
			}

			// proceed to next level of the trees
		}



		int a = 5;
		
	}




}