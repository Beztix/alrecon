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

#include "rec_container.h"


namespace rec {

	void createObject3DTree(std::vector<viral_core::vector> cameraPositions, std::vector<std::vector<std::vector<viral_core::vector>>*> directionsGrids, 
			int nrOfCams, int width, int height) {

		// list of all containerLists, one per camera
		std::vector<std::vector<rec::container>> containerLists;


		// ==== fill container Lists with containers representing the whole images ==== 

		// create corners
		viral_core::vector2f corner1(width, 0);
		viral_core::vector2f corner2(0,0);
		viral_core::vector2f corner3(0, height);
		viral_core::vector2f corner4(width, height);
		
		for (int i = 0; i < nrOfCams; i++) {
			std::vector<rec::container> currentContainerList;
			
			//create container
			rec::container currentCont(corner1, corner2, corner3, corner4);

			//calculate frustum
			std::vector<std::vector<viral_core::vector>>* directionsGrid = directionsGrids.at(i);
			viral_core::vector camPosition = cameraPositions.at(i);
			viral_core::vector direction1 = (*directionsGrid).at(corner1.y).at(corner1.x);
			viral_core::vector direction2 = (*directionsGrid).at(corner2.y).at(corner2.x);
			viral_core::vector direction3 = (*directionsGrid).at(corner3.y).at(corner3.x);
			viral_core::vector direction4 = (*directionsGrid).at(corner4.y).at(corner4.x);
			viral_core::vector near_top_left = camPosition + (direction2 * 70);
			viral_core::vector near_top_right = camPosition + (direction1 * 70);
			viral_core::vector near_bot_left = camPosition + (direction3 * 70);
			viral_core::vector near_bot_right = camPosition + (direction4 * 70);
			viral_core::vector far_top_left = camPosition + (direction2 * 500);
			viral_core::vector far_top_right = camPosition + (direction1 * 500);
			viral_core::vector far_bot_left = camPosition + (direction3 * 500);
			viral_core::vector far_bot_right = camPosition + (direction4 * 500);

			rec::frustum currentFrust = rec::frustum(near_bot_left, near_bot_right, near_top_right, near_top_left,
													far_bot_left, far_bot_right, far_top_right, far_top_left);
			currentCont.setFrustum(currentFrust);
			currentContainerList.push_back(currentCont);

			containerLists.push_back(currentContainerList);
		}






	}




}