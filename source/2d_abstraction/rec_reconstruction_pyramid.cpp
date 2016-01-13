/**
*************************************************************************
*
* @file rec_reconstruction_pyramid.cpp
*
* 
*
************************************************************************/

#include "rec_reconstruction_pyramid.h"


#include <vector>
#include <iostream>


#include "rec_object3D.h"
#include "rec_pyramid_intersection.h"
#include "tree.hh"
#include "text_output.h"



namespace rec {




	tree<rec::object3D> createObject3DTree(std::vector<viral_core::vector> cameraPositions, std::vector<std::vector<std::vector<viral_core::vector>>> &directionsGrids,
		std::vector<tree<rec::seAndPyramid>> &seAndPyramidTrees, std::vector<std::vector<int>> treeContentCounts, int nrOfCams, int width, int height, int offset, rec::aabb workspace) {

		double CAM_NEAR_PLANE = 500;
		double CAM_FAR_PLANE = 8000;


		int depthOfSeAndPyramidTrees = seAndPyramidTrees.at(0).max_depth();

		tree<rec::object3D> object3DTree;
		object3DTree.clear();

		// list of all seAndPyramidPLists, one per camera
		// these lists contain pointers to the objects to be intersected
		std::vector<std::vector<rec::seAndPyramid*>> seAndPyramidPLists;


		std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> seAndPyramidITLists;




		//calculate total content count

		std::vector<int> totalContentCount;
		for (int i = 0; i < treeContentCounts.at(0).size(); i++) {
			totalContentCount.push_back(0);
		}

		for (int cam = 1; cam <= nrOfCams; cam++) {
			std::vector<int> treeContentCount = treeContentCounts.at(cam - 1);
		
			for (int i = 0; i < treeContentCount.size(); i++) {
				totalContentCount[i] = totalContentCount[i] + treeContentCount[i];
			}
		}





		//====================================
		//===    Root Nodes of 2D Trees    ===
		//====================================

		int usedTotalIDs = 0;

		// for each camera
		for (int cam = 1; cam <= nrOfCams; cam++) {
			tree<rec::seAndPyramid>::pre_order_iterator seAndPyramidTreeTop = seAndPyramidTrees.at(cam - 1).begin();
			std::vector<int> treeContentCount = treeContentCounts.at(cam - 1);

			// list of pointers to the current seAndPyramids
			std::vector<rec::seAndPyramid*> currentSeAndPyramidPList;


			std::vector<tree<rec::seAndPyramid>::pre_order_iterator> currentSeAndPyramidItList;
			
			// get reference to rootSeAndPyramid
			rec::seAndPyramid& rootSeAndPyramid = *seAndPyramidTreeTop;

			//calculate Pyramid of the root node (which represents the whole camera viewing Pyramid)
			viral_core::vector camPosition = cameraPositions.at(cam - 1);
			viral_core::vector direction1 = directionsGrids.at(cam - 1).at(rootSeAndPyramid.corner1.y + rootSeAndPyramid.offSetY + offset).at(rootSeAndPyramid.corner1.x + rootSeAndPyramid.offSetX + offset);
			viral_core::vector direction2 = directionsGrids.at(cam - 1).at(rootSeAndPyramid.corner2.y + rootSeAndPyramid.offSetY + offset).at(rootSeAndPyramid.corner2.x + rootSeAndPyramid.offSetX + offset);
			viral_core::vector direction3 = directionsGrids.at(cam - 1).at(rootSeAndPyramid.corner3.y + rootSeAndPyramid.offSetY + offset).at(rootSeAndPyramid.corner3.x + rootSeAndPyramid.offSetX + offset);
			viral_core::vector direction4 = directionsGrids.at(cam - 1).at(rootSeAndPyramid.corner4.y + rootSeAndPyramid.offSetY + offset).at(rootSeAndPyramid.corner4.x + rootSeAndPyramid.offSetX + offset);
			
			viral_core::vector bot_left = camPosition + (direction4 * CAM_FAR_PLANE);
			viral_core::vector bot_right = camPosition + (direction3 * CAM_FAR_PLANE);
			viral_core::vector top_right = camPosition + (direction2 * CAM_FAR_PLANE);
			viral_core::vector top_left = camPosition + (direction1 * CAM_FAR_PLANE);

			rec::pyramid currentPyramid = rec::pyramid(bot_left, bot_right, top_right, top_left, camPosition);
			currentPyramid.setID(usedTotalIDs + rootSeAndPyramid.camInternalID);
			usedTotalIDs += treeContentCount.at(0);

			rootSeAndPyramid.setPyramid(currentPyramid);

			//text_output::appendPyramidToTextFile("Pyramids_cam" + std::to_string(cam) + ".txt", currentPyramid);

			// add pointer to the rootSeAndPyramid to the list of current seAndPyramids
			currentSeAndPyramidPList.push_back(&rootSeAndPyramid);


			currentSeAndPyramidItList.push_back(seAndPyramidTreeTop);


			// add the list of pointers of the current seAndPyramids to the list of seAndPyramidPList
			seAndPyramidPLists.push_back(currentSeAndPyramidPList);


			seAndPyramidITLists.push_back(currentSeAndPyramidItList);
		}

		

		// ###### intersect initial seAndPyramidITLists ######
		std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> intersectionIT = rec::intersectAllPyramids(seAndPyramidITLists, workspace);
		seAndPyramidITLists.clear();


		//intersection of the whole camera viewing Pyramid should be one object
		if (intersectionIT.size() != 1) {
			//ERROR
			//TODO: WHAT NOW?
			return object3DTree;
		}

		//add root node to the tree
		rec::object3D rootObject = object3D(intersectionIT.at(0));
		object3DTree.set_head(rootObject);
		tree<rec::object3D>::iterator object3DTreeTop = object3DTree.begin();






		//================================================
		//===    Process further levels of 2D trees    ===
		//================================================



		// as long as there are unprocessed levels of the 2D trees left:
		//for (int currentLevel = 0; currentLevel < depthOfSeAndPyramidTrees; currentLevel++) {
		for (int currentLevel = 0; currentLevel < 3; currentLevel++) {
			std::cout << std::endl;
			std::cout << "==== processing level " + std::to_string(currentLevel) + " ====" << std::endl;
			std::cout << std::endl;


			tree<rec::object3D>::fixed_depth_iterator currentLevel3DIterator = object3DTree.begin_fixed(object3DTreeTop, currentLevel);
			int objectsOnNextLevel = 0;
			int object = 0;
			// for each object3D on the current level of the 3D tree
			while (object3DTree.is_valid(currentLevel3DIterator)) {
				
				if (object % 10 == 0) {
					std::cout << "==== processing object " + std::to_string(object) + " on level " + std::to_string(currentLevel) + " ====" << std::endl;
				}
				
				// get reference to current object3D
				rec::object3D& currentObject3D = *currentLevel3DIterator;

				// process each camera = each generating Pyramid 
				// (generating Pyramids live on currentLevel of the 2D trees)
				for (int cam = 1; cam <= nrOfCams; cam++) {

					tree<rec::seAndPyramid>& currentSeAndPyramidTree = seAndPyramidTrees.at(cam-1);

					// list containing the Pyramids of the current camera which should be included in the next Pyramid intersections step
					std::vector<tree<rec::seAndPyramid>::pre_order_iterator> currentSeAndPyramidITList;

					// generating Pyramid of the current 3D object of the current camera
					tree<rec::seAndPyramid>::pre_order_iterator currentSeAndPyramidIT = currentObject3D.generatingPyramidsIT.at(cam-1);

					// create iterator of the children of currentSeAndPyramid, which are the new Pyramids to be tested
					// (they live on currentLevel + 1 of the 2D trees)
					tree<rec::seAndPyramid>::sibling_iterator childrenITbegin = currentSeAndPyramidIT.begin();
					tree<rec::seAndPyramid>::sibling_iterator childrenITend = currentSeAndPyramidIT.end();
					
					// add all children to the currentSeAndPyramidITList
					while (childrenITbegin != childrenITend) {

						rec::seAndPyramid& currentChild = *childrenITbegin;

						//calculate Pyramid of the current child
						viral_core::vector camPosition = cameraPositions.at(cam-1);
						viral_core::vector direction1 = directionsGrids.at(cam - 1).at(currentChild.corner1.y + currentChild.offSetY + offset).at(currentChild.corner1.x + currentChild.offSetX + offset);
						viral_core::vector direction2 = directionsGrids.at(cam - 1).at(currentChild.corner2.y + currentChild.offSetY + offset).at(currentChild.corner2.x + currentChild.offSetX + offset);
						viral_core::vector direction3 = directionsGrids.at(cam - 1).at(currentChild.corner3.y + currentChild.offSetY + offset).at(currentChild.corner3.x + currentChild.offSetX + offset);
						viral_core::vector direction4 = directionsGrids.at(cam - 1).at(currentChild.corner4.y + currentChild.offSetY + offset).at(currentChild.corner4.x + currentChild.offSetX + offset);
						
						viral_core::vector bot_left = camPosition + (direction4 * CAM_FAR_PLANE);
						viral_core::vector bot_right = camPosition + (direction3 * CAM_FAR_PLANE);
						viral_core::vector top_right = camPosition + (direction2 * CAM_FAR_PLANE);
						viral_core::vector top_left = camPosition + (direction1 * CAM_FAR_PLANE);
						
						rec::pyramid currentPyramid = rec::pyramid(bot_left, bot_right, top_right, top_left, camPosition);
						currentChild.setPyramid(currentPyramid);

						//text_output::appendPyramidToTextFile("Pyramids_cam" + std::to_string(cam) + ".txt", currentPyramid);


						currentSeAndPyramidITList.push_back(tree<rec::seAndPyramid>::pre_order_iterator(childrenITbegin));
						childrenITbegin++;
					}

					seAndPyramidITLists.push_back(currentSeAndPyramidITList);
				}


				// ###### intersect current seAndPyramidITLists ######
				intersectionIT = rec::intersectAllPyramids(seAndPyramidITLists, workspace);
				//std::cout << "intersection finished, Nr of new 3D children: " + std::to_string(intersectionIT.size()) << std::endl;
				seAndPyramidITLists.clear();

				// add all objects received from intersection as children of the currentObject3D to the object3DTree
				for (int i = 0; i < intersectionIT.size(); i++) {
					rec::object3D currentIntersectionObject = object3D(intersectionIT.at(i));
					object3DTree.append_child(currentLevel3DIterator, currentIntersectionObject);
				}
				

				// proceed to next object of the current level
				currentLevel3DIterator++;
				object++;
				objectsOnNextLevel += intersectionIT.size();
			}

			std::cout << std::endl;
			std::cout << "==== total number of new objects on level " + std::to_string(currentLevel+1) + ": " + std::to_string(objectsOnNextLevel) + " ====" << std::endl;
			std::cout << std::endl;

			// proceed to next level of the trees
		}

		return object3DTree;
	}













	std::vector<viral_core::vector> reconstruct_object3DTree(int stepsize, std::vector<rec::sensor> sensors, tree<rec::object3D> object3DTree, int level) {

		std::vector<viral_core::vector> occupiedWorldPositions;

		tree<rec::object3D>::iterator object3DTreeTop = object3DTree.begin();
		


		std::cout << "Testing all world space positions for being occupied in the various 3D object" << std::endl;

		bool firstIn = false;

		// run through all positions in world space
		for (int x = -2000; x < 2200; x += stepsize) {
			if (x % 1 == 0) {
				std::cout << "testing x = " + std::to_string(x) << std::endl;
			}
			for (int y = -2200; y < 2200; y += stepsize) {
				for (int z = -880; z < 1600; z += stepsize) {
					/*
					if (x % 1 == 0) {
						std::cout << "testing x = " + std::to_string(x) + "y = " + std::to_string(y) + "z = " + std::to_string(z) << std::endl;
					}
					*/
					viral_core::vector vec((float)x, (float)y, (float)z);

					//test if world space positions are occupied according to object3DTree
				
					bool inside = false;

					int o = 0;
					tree<rec::object3D>::fixed_depth_iterator currentLevel3DIterator = object3DTree.begin_fixed(object3DTreeTop, level);
					while (object3DTree.is_valid(currentLevel3DIterator)) {

						rec::object3D& currentObject3D = *currentLevel3DIterator;

					
						//std::cout << "testing object " + std::to_string(o) << std::endl;

						if (currentObject3D.isPointInside(vec)) {
							
							inside = true;
							if (!firstIn) {
								std::cout << "inside at object " + std::to_string(o) << std::endl;
								firstIn = true;
							}
							break;
						}
						currentLevel3DIterator++;
						o++;
					}


					if (inside) {
						//std::cout << "inside" << std::endl;
						occupiedWorldPositions.emplace_back(vec);
					}
				}
			}
		}


		return occupiedWorldPositions;

	}







	std::vector<std::vector<viral_core::vector>> reconstruct_object3DTree_objectSeparated(rec::aabb workspace,int stepsize, std::vector<rec::sensor> sensors, tree<rec::object3D> object3DTree, int level) {

		std::vector<std::vector<viral_core::vector>> allOccupiedWorldPositions;

		tree<rec::object3D>::iterator object3DTreeTop = object3DTree.begin();



		std::cout << "Testing all world space positions for being occupied in the various 3D object" << std::endl;



		int o = 0;
		tree<rec::object3D>::fixed_depth_iterator currentLevel3DIterator = object3DTree.begin_fixed(object3DTreeTop, level);


		// run through all objects at the desired level
		while (object3DTree.is_valid(currentLevel3DIterator)) {
			rec::object3D& currentObject3D = *currentLevel3DIterator;

			std::vector<viral_core::vector> currentOccupiedWorldPositions;

			//text_output::writeObject3DToTextFile("object_" + std::to_string(o) + ".txt", currentObject3D);


			if (!(o == 100)) {

				//text_output::writeObject3DToPyramidTextFiles("testObject_pyr", currentObject3D);

				// run through all world space positions
				for (int x = workspace.extremalValues[rec::aabb::minX]; x < workspace.extremalValues[rec::aabb::maxX]; x += stepsize) {
					if (x % 100 == 0) {
						std::cout << "testing x = " + std::to_string(x) << std::endl;
					}
					for (int y = workspace.extremalValues[rec::aabb::minY]; y < workspace.extremalValues[rec::aabb::maxY]; y += stepsize) {
						for (int z = workspace.extremalValues[rec::aabb::minZ]; z < workspace.extremalValues[rec::aabb::maxZ]; z += stepsize) {
							/*
							if (x % 1 == 0) {
							std::cout << "testing x = " + std::to_string(x) + "y = " + std::to_string(y) + "z = " + std::to_string(z) << std::endl;
							}
							*/
							viral_core::vector vec((float)x, (float)y, (float)z);

							//test if world space positions are occupied according to object3DTree


							//std::cout << "testing object " + std::to_string(o) << std::endl;

							if (currentObject3D.isPointInside(vec)) {
								currentOccupiedWorldPositions.emplace_back(vec);

							}
						}
					}
				}
			}

			allOccupiedWorldPositions.push_back(currentOccupiedWorldPositions);

			currentLevel3DIterator++;
			o++;

		}

		return allOccupiedWorldPositions;

	}

}