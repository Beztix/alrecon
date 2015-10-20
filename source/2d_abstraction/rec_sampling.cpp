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





namespace rec {


	std::vector<viral_core::vector> sample_camera(file_camera f, sensor s, int index) {

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
		int stepsX = 300;
		int stepsY = 300;
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
		image_output::pixelVectorToImage(pixels, 640, 480, "projectionTestPlane" + std::to_string(index) + ".png");


		std::vector<viral_core::vector> vectors;
		vectors.emplace_back(topLeftPlaneVector*0.1);
		vectors.emplace_back(topRightPlaneVector*0.1);
		vectors.emplace_back(botLeftPlaneVector*0.1);
		vectors.emplace_back(botRightPlaneVector*0.1);

		return vectors;
	}

}