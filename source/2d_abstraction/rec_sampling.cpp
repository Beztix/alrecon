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
#include "text_input.h"

#include <iostream>

namespace rec {


	std::vector<viral_core::vector> sample_camera_to_image(file_camera f, sensor s, float distance) {

		viral_core::vector positionVector = f.cam_position_;
		viral_core::rotation rotation = f.cam_inv_orientation_;

		// get the three coordinate axes of the camera in world space
		viral_core::vector rotationVectorX = rotation * viral_core::vector(1, 0, 0);
		viral_core::vector rotationVectorY = rotation * viral_core::vector(0, 1, 0);
		viral_core::vector rotationVectorZ = rotation * viral_core::vector(0, 0, 1);

		//calculate a plane in front of the camera
		viral_core::vector centralPlaneVector = positionVector + rotationVectorZ * distance;
		float sizeX = distance*3;
		float sizeY = distance*3;
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
		cornerVectors.emplace_back(topLeftPlaneVector*0.1f);
		cornerVectors.emplace_back(topRightPlaneVector*0.1f);
		cornerVectors.emplace_back(botLeftPlaneVector*0.1f);
		cornerVectors.emplace_back(botRightPlaneVector*0.1f);

		return cornerVectors;
	}




	std::vector<std::vector<viral_core::vector>> sample_camera_for_inv_projection(file_camera f, sensor s, float distance) {

		std::cout << "starting sampling of a plane in front of camera" + std::to_string(f.index) << std::endl;

		viral_core::vector positionVector = f.cam_position_;
		viral_core::rotation rotation = f.cam_inv_orientation_;

		// get the three coordinate axes of the camera in world space
		viral_core::vector rotationVectorX = rotation * viral_core::vector(1, 0, 0);
		viral_core::vector rotationVectorY = rotation * viral_core::vector(0, 1, 0);
		viral_core::vector rotationVectorZ = rotation * viral_core::vector(0, 0, 1);

		//calculate a plane in front of the camera
		viral_core::vector centralPlaneVector = positionVector + rotationVectorZ * distance;
		float sizeX = distance*3;
		float sizeY = distance*3;
		viral_core::vector topLeftPlaneVector = centralPlaneVector - rotationVectorX * sizeX - rotationVectorY * sizeY;
		viral_core::vector topRightPlaneVector = centralPlaneVector + rotationVectorX * sizeX - rotationVectorY * sizeY;
		viral_core::vector botLeftPlaneVector = centralPlaneVector - rotationVectorX * sizeX + rotationVectorY * sizeY;
		viral_core::vector botRightPlaneVector = centralPlaneVector + rotationVectorX * sizeX + rotationVectorY * sizeY;


		//sample the plane with x and y steps
		int stepsX = 15000;
		int stepsY = 15000;
		viral_core::vector leftToRight = topRightPlaneVector - topLeftPlaneVector;
		viral_core::vector topToBot = botLeftPlaneVector - topLeftPlaneVector;
		viral_core::vector stepVectorX = leftToRight / (float)stepsX;
		viral_core::vector stepVectorY = topToBot / (float)stepsY;

		//CAUTION: SMAPLING WITH 300PX OUTSIDE OF THE PICTURE IN EACH DIRECTION
		//-> OFFSET OF 300PX

		//a 2D std::vector grid representing the pixels, which has a std::vector of viral_core::vectors for each pixel
		std::vector<std::vector<std::vector<viral_core::vector>>> pixelGridWithPositions(1080, std::vector<std::vector<viral_core::vector>>(1240, std::vector<viral_core::vector>()));

		//sample the plane
		for (int i = 0; i < stepsY; i++) {
			if (i % 100 == 0) {
				std::cout << "sampling row " + std::to_string(i) << std::endl;
			}
			for (int k = 0; k < stepsX; k++) {

				viral_core::vector vec = topLeftPlaneVector + stepVectorY * (float)i + stepVectorX * (float)k;
				viral_core::vector projVec = s.project(vec);
				int xWithOffset = (int)(300 + projVec.x);
				int yWithOffset = (int)(300 + projVec.y);

				if (0 <= xWithOffset && xWithOffset < 1240 && 0 <= yWithOffset && yWithOffset < 1080) {
					//add the positions which are projected into the image to the std::vector of positions of the corresponding pixel
					pixelGridWithPositions.at(yWithOffset).at(xWithOffset).emplace_back(vec);
				}
			}
		}


		//create the average sampled position for each pixel
		std::vector<std::vector<viral_core::vector>> pixelGridWithAveragePosition(1080, std::vector<viral_core::vector>(1240, viral_core::vector()));
		
		for (int y = 0; y < 1080; y++) {
			for (int x = 0; x < 1240; x++) {
				std::vector<viral_core::vector> positionsOfCurrentPixel = pixelGridWithPositions.at(y).at(x);

				//sum up the vectors of the current pixel
				viral_core::vector totalPositionVector(0, 0, 0);
				for (int i = 0; i < positionsOfCurrentPixel.size(); i++) {
					totalPositionVector = totalPositionVector + positionsOfCurrentPixel.at(i);
				}
				//calculate average
				viral_core::vector averagePosition = totalPositionVector / (float)positionsOfCurrentPixel.size();
				pixelGridWithAveragePosition[y][x] = averagePosition;
			}
		}


		//write averages to file
		std::string locationString = "../../assets/camera_inversion/sampledPositions_d" + std::to_string((int)distance) + "_cam" + std::to_string(f.index);

		text_output::writePositionsGridToTextfile(locationString + ".txt", pixelGridWithAveragePosition);
		text_output::writePositionsGridToBinaryfile(locationString + ".bin", pixelGridWithAveragePosition);

		return pixelGridWithAveragePosition;

	
	}






	std::vector<std::vector<viral_core::vector>> calculateNormalizedDirectionvectors(viral_core::vector camPosition, int distance, int index, int sizeX, int sizeY) {

		std::cout << "starting calculation of normalized direction vectors of camera" + std::to_string(index) << std::endl;

		std::string locationString = "../../assets/camera_inversion/sampledPositions_d" + std::to_string(distance) + "_cam" + std::to_string(index) + ".bin";
		std::vector<std::vector<viral_core::vector>> positionGrid = text_input::readPositionsGridFromBinaryfile(locationString, sizeX, sizeY);
		std::vector<std::vector<viral_core::vector>> directionsGrid(sizeY, std::vector<viral_core::vector>(sizeX, viral_core::vector()));


		for (int y = 0; y < sizeY; y++) {
			if (y % 100 == 0) {
				std::cout << "calculating row " + std::to_string(y) << std::endl;
			}
			for (int x = 0; x < sizeX; x++) {
				viral_core::vector direction = positionGrid.at(y).at(x) - camPosition;
				direction = direction / distance;
				directionsGrid[y][x] = direction;
			}
		}


		std::string destString = "../../assets/camera_inversion/directions_distanceNormalized_cam" + std::to_string(index);
		text_output::writePositionsGridToTextfile(destString + ".txt", directionsGrid);
		text_output::writePositionsGridToBinaryfile(destString + ".bin", directionsGrid);

		return directionsGrid;
	}





	void doAllSamplingCalculations() {


		for (int i = 0; i < 7; i++) {

			file_camera current_cam(i);
			sensor current_sensor(i, current_cam.image_size, sensor::projection_pinhole_distort);
			current_sensor.set_pinhole_distort(current_cam.world_to_device_pinhole_distort_, current_cam.pinhole_distort_center_, current_cam.pinhole_distort_focus_,
				current_cam.distort_r1_, current_cam.distort_r2_, current_cam.distort_t1_, current_cam.distort_t2_);

		
			/*
			rec::sample_camera_to_image(current_cam, current_sensor, 500);
			rec::sample_camera_for_inv_projection(current_cam, current_sensor, 700);
			*/

			calculateNormalizedDirectionvectors(current_cam.cam_position_, 700, i, 1240, 1080);

		

		
		}

	}



}