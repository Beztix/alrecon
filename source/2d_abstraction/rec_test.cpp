/**
*************************************************************************
*
* @file rec_test.cpp
*
* 
*
************************************************************************/

#include "rec_test.h"


#include "text_input.h"
#include "rec_pyramid_intersection.h"




namespace rec {

	

	std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> testPyramidsWithGivenPoint(viral_core::vector p) {
		std::vector<tree<rec::seAndPyramid>> seAndPyramidTrees;

		std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> cameraPyramidLists;



		//for each camera, add one pyramid containing p to cameraPyramids
		for (int i = 1; i <= 7; i++) {
			std::vector<rec::pyramid> pyramids = text_input::readPyramidsFromTextfile("Pyramids_cam" + std::to_string(i) + ".txt");

			for (rec::pyramid currentPyr : pyramids) {
				if (currentPyr.isPointInside(p)) {
					//create seAndPyramid tree of current cam, add to list
					rec::seAndPyramid currentSeAndPyramid = rec::seAndPyramid(currentPyr);
					tree<rec::seAndPyramid> currentTree;
					seAndPyramidTrees.push_back(currentTree);
					seAndPyramidTrees.at(i - 1).set_head(currentSeAndPyramid);
					break;
				}
			}
		}

		bool inPyramids1 = true;
		for (tree<rec::seAndPyramid> currentTree : seAndPyramidTrees) {
			tree<rec::seAndPyramid>::iterator treeTop = currentTree.begin();
			rec::seAndPyramid currentSeAndPyr = (*treeTop);
			rec::pyramid currentPyr = currentSeAndPyr.pyr;
			if (!currentPyr.isPointInside(p)) {
				inPyramids1 = false;
			}
		}

		for (int i = 1; i <= 7; i++) {

			//get iterator to the pyramid
			tree<rec::seAndPyramid>::iterator top = seAndPyramidTrees.at(i - 1).begin();
			tree<rec::seAndPyramid>::pre_order_iterator currentIt = tree<rec::seAndPyramid>::pre_order_iterator(top);

			//save it in the lists
			std::vector<tree<rec::seAndPyramid>::pre_order_iterator> currentCamList;
			currentCamList.push_back(currentIt);
			cameraPyramidLists.push_back(currentCamList);

		}
		
		bool inPyramids2 = true;
		for (std::vector<tree<rec::seAndPyramid>::pre_order_iterator> currentPyrItList : cameraPyramidLists) {
			tree<rec::seAndPyramid>::pre_order_iterator currentPyrIt = currentPyrItList.at(0);
			rec::seAndPyramid currentSeAndPyr = (*currentPyrIt);
			rec::pyramid currentPyr = currentSeAndPyr.pyr;
			if (!currentPyr.isPointInside(p)) {
				inPyramids2 = false;
			}
		}


		rec::aabb workspace = rec::aabb(-2500, 2700, -2700, 2700, -1080, 2000);

		std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> intersectionIT = intersectAllPyramids(cameraPyramidLists, workspace);

		bool inPyramids3 = true;
		std::vector<tree<rec::seAndPyramid>::pre_order_iterator> firstIntersectionIT = intersectionIT.at(0);
		for (tree<rec::seAndPyramid>::pre_order_iterator currentPyrIt : firstIntersectionIT) {
			rec::seAndPyramid currentSeAndPyr = (*currentPyrIt);
			rec::pyramid currentPyr = currentSeAndPyr.pyr;
			if (!currentPyr.isPointInside(p)) {
				inPyramids3 = false;
			}
		}

		
		return intersectionIT;
	}






	std::vector<std::vector<viral_core::vector>> testReconstruct(int stepsize, std::vector<rec::sensor> sensors, std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> intersectionIT) {
		std::vector<std::vector<viral_core::vector>> allOccupiedWorldPositions;

		


		std::cout << "Testing all world space positions for being occupied in the various 3D object" << std::endl;



		int o = 0;


		// run through all objects at the desired level
		for(std::vector<tree<rec::seAndPyramid>::pre_order_iterator> currentITlist : intersectionIT) {

			rec::object3D currentObject = rec::object3D(currentITlist);

			std::vector<viral_core::vector> currentOccupiedWorldPositions;


			//text_output::writeObject3DToPyramidTextFiles("testObject_pyr", currentObject3D);

			// run through all world space positions
			for (int x = -3000; x < 3200; x += stepsize) {
				if (x % 100 == 0) {
					std::cout << "testing x = " + std::to_string(x) << std::endl;
				}
				for (int y = -3200; y < 3200; y += stepsize) {
					for (int z = -1880; z < 1600; z += stepsize) {
						/*
						if (x % 1 == 0) {
						std::cout << "testing x = " + std::to_string(x) + "y = " + std::to_string(y) + "z = " + std::to_string(z) << std::endl;
						}
						*/
						viral_core::vector vec((float)x, (float)y, (float)z);

						//test if world space positions are occupied according to object3DTree


						//std::cout << "testing object " + std::to_string(o) << std::endl;

						if (currentObject.isPointInside(vec)) {
							currentOccupiedWorldPositions.emplace_back(vec);

						}
					}
				}
			}

			allOccupiedWorldPositions.push_back(currentOccupiedWorldPositions);

			o++;
		}




		return allOccupiedWorldPositions;
	}



}