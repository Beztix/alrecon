/**
*************************************************************************
*
* @file rec_sampling.cpp
*
* 
*
************************************************************************/

#include "rec_sampling.h"
#include "image_output.h"
#include "text_output.h"




namespace rec {


	std::vector<viral_core::vector> sample_camera_to_image(file_camera f, sensor s) {

		viral_core::vector positionVector = f.cam_position_;
		viral_core::rotation rotation = f.cam_inv_orientation_;

		// get the three coordinate axes of the camera in world space
		viral_core::vector rotationVectorX = rotation * viral_core::vector(1, 0, 0);
		viral_core::vector rotationVectorY = rotation * viral_core::vector(0, 1, 0);
		viral_core::vector rotationVectorZ = rotation * viral_core::vector(0, 0, 1);

		//calculate a plane in front of the camera
		viral_core::vector centralPlaneVector = positionVector + rotationVectorZ * 500;
		float sizeX = 700;
		float sizeY = 500;
		viral_core::vector topLeftPlaneVector = centralPlaneVector - rotationVectorX * sizeX - rotationVectorY * sizeY;
		viral_core::vector topRightPlaneVector = centralPlaneVector + rotationVectorX * sizeX - rotationVectorY * sizeY;
		viral_core::vector botLeftPlaneVector = centralPlaneVector - rotationVectorX * sizeX + rotationVectorY * sizeY;
		viral_core::vector botRightPlaneVector = centralPlaneVector + rotationVectorX * sizeX + rotationVectorY * sizeY;
	

		//sample the plane with x and y steps
		int stepsX = 100;
		int stepsY = 100;
		viral_core::vector leftToRight = topRightPlaneVector - topLeftPlaneVector;
		viral_core::vector topToBot = botLeftPlaneVector - topLeftPlaneVector;
		viral_core::vector stepVectorX = leftToRight / (float)stepsX;
		viral_core::vector stepVectorY = topToBot / (float)stepsY;

		std::vector<int> pixels;

		for (int i = 0; i < stepsY; i++) {
			for (int k = 0; k < stepsX; k++) {
				viral_core::vector vec = topLeftPlaneVector + stepVectorY * (float)i + stepVectorX * (float)k;
				viral_core::vector projVec = s.project(vec);
				if (0 <= projVec.x && projVec.x < 640 && 0 <= projVec.y && projVec.y < 480) {
					pixels.emplace_back((int)projVec.x);
					pixels.emplace_back((int)projVec.y);
				}
			}
			
		}
		image_output::pixelVectorToImage(pixels, 640, 480, "projectionTestPlane" + std::to_string(f.index) + ".png");


		std::vector<viral_core::vector> cornerVectors;
		cornerVectors.emplace_back(topLeftPlaneVector*0.1);
		cornerVectors.emplace_back(topRightPlaneVector*0.1);
		cornerVectors.emplace_back(botLeftPlaneVector*0.1);
		cornerVectors.emplace_back(botRightPlaneVector*0.1);

		return cornerVectors;
	}




	std::vector<std::vector<viral_core::vector>> sample_camera_for_inv_projection(file_camera f, sensor s) {

		viral_core::vector positionVector = f.cam_position_;
		viral_core::rotation rotation = f.cam_inv_orientation_;

		// get the three coordinate axes of the camera in world space
		viral_core::vector rotationVectorX = rotation * viral_core::vector(1, 0, 0);
		viral_core::vector rotationVectorY = rotation * viral_core::vector(0, 1, 0);
		viral_core::vector rotationVectorZ = rotation * viral_core::vector(0, 0, 1);

		//calculate a plane in front of the camera
		viral_core::vector centralPlaneVector = positionVector + rotationVectorZ * 500;
		float sizeX = 700;
		float sizeY = 500;
		viral_core::vector topLeftPlaneVector = centralPlaneVector - rotationVectorX * sizeX - rotationVectorY * sizeY;
		viral_core::vector topRightPlaneVector = centralPlaneVector + rotationVectorX * sizeX - rotationVectorY * sizeY;
		viral_core::vector botLeftPlaneVector = centralPlaneVector - rotationVectorX * sizeX + rotationVectorY * sizeY;
		viral_core::vector botRightPlaneVector = centralPlaneVector + rotationVectorX * sizeX + rotationVectorY * sizeY;


		//sample the plane with x and y steps
		int stepsX = 5000;
		int stepsY = 5000;
		viral_core::vector leftToRight = topRightPlaneVector - topLeftPlaneVector;
		viral_core::vector topToBot = botLeftPlaneVector - topLeftPlaneVector;
		viral_core::vector stepVectorX = leftToRight / (float)stepsX;
		viral_core::vector stepVectorY = topToBot / (float)stepsY;

		//a 2D std::vector grid representing the pixels, which has a std::vector of viral_core::vectors for each pixel
		std::vector<std::vector<std::vector<viral_core::vector>>> pixelGridWithPositions(480, std::vector<std::vector<viral_core::vector>>(640, std::vector<viral_core::vector>()));

		//sample the plane
		for (int i = 0; i < stepsY; i++) {
			for (int k = 0; k < stepsX; k++) {
				viral_core::vector vec = topLeftPlaneVector + stepVectorY * (float)i + stepVectorX * (float)k;
				viral_core::vector projVec = s.project(vec);
				if (0 <= projVec.x && projVec.x < 640 && 0 <= projVec.y && projVec.y < 480) {
					//add the positions which are projected into the image to the std::vector of positions of the corresponding pixel
					pixelGridWithPositions.at(projVec.y).at(projVec.x).emplace_back(vec);
				}
			}

		}


		std::vector<std::vector<viral_core::vector>> pixelGridWithAveragePosition(480, std::vector<viral_core::vector>(640, viral_core::vector()));
		
		for (int y = 0; y < 480; y++) {
			for (int x = 0; x < 640; x++) {
				std::vector<viral_core::vector> positionsOfCurrentPixel = pixelGridWithPositions.at(y).at(x);

				//sum up the vectors of the current pixel
				viral_core::vector totalPositionVector(0, 0, 0);
				for (int i = 0; i < positionsOfCurrentPixel.size(); i++) {
					totalPositionVector = totalPositionVector + positionsOfCurrentPixel.at(i);
				}

				viral_core::vector averagePosition = totalPositionVector / positionsOfCurrentPixel.size();

				pixelGridWithAveragePosition[y][x] = averagePosition;

			}
		}


		text_output::writePositionsGridToTextfile(pixelGridWithAveragePosition, f.index);

		return pixelGridWithAveragePosition;

	
	}










}