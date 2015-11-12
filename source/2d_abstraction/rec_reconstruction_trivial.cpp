/**
*************************************************************************
*
* @file rec_reconstruction_trivial.cpp
*
* 
*
************************************************************************/

#include "rec_reconstruction_trivial.h"


#include "image_input.h"

// Viral
#include <viral_core/engine.hpp>
#include <viral_core/window.hpp>
#include <viral_core/input.hpp>
#include <viral_core/file.hpp>
#include <viral_core/timer.hpp>
#include <viral_core/image.hpp>

#include <viral_core/render_resource.hpp>
#include <viral_core/render_scene.hpp>
#include <viral_core/render_process.hpp>
#include <viral_core/render_command.hpp>
#include <viral_core/render_task.hpp>
#include <viral_core/render.hpp>



#include "rec_file_camera.h"
#include "rec_sensor.h"

#include <vector>



namespace rec {






	std::vector<viral_core::vector> reconstruct_trivial(int stepsize) {


		//create file_cameras and sensors using stefan's camera calibration to be able to project from world to device

		file_camera cam_0(0);
		sensor sens_0(0, cam_0.image_size, sensor::projection_pinhole_distort);
		sens_0.set_pinhole_distort(cam_0.world_to_device_pinhole_distort_, cam_0.pinhole_distort_center_, cam_0.pinhole_distort_focus_, 
								   cam_0.distort_r1_, cam_0.distort_r2_, cam_0.distort_t1_, cam_0.distort_t2_);

		file_camera cam_1(1);
		sensor sens_1(1, cam_1.image_size, sensor::projection_pinhole_distort);
		sens_1.set_pinhole_distort(cam_1.world_to_device_pinhole_distort_, cam_1.pinhole_distort_center_, cam_1.pinhole_distort_focus_,
			cam_1.distort_r1_, cam_1.distort_r2_, cam_1.distort_t1_, cam_1.distort_t2_);

		file_camera cam_2(2);
		sensor sens_2(2, cam_2.image_size, sensor::projection_pinhole_distort);
		sens_2.set_pinhole_distort(cam_2.world_to_device_pinhole_distort_, cam_2.pinhole_distort_center_, cam_2.pinhole_distort_focus_,
			cam_2.distort_r1_, cam_2.distort_r2_, cam_2.distort_t1_, cam_2.distort_t2_);

		file_camera cam_3(3);
		sensor sens_3(3, cam_3.image_size, sensor::projection_pinhole_distort);
		sens_3.set_pinhole_distort(cam_3.world_to_device_pinhole_distort_, cam_3.pinhole_distort_center_, cam_3.pinhole_distort_focus_,
			cam_3.distort_r1_, cam_3.distort_r2_, cam_3.distort_t1_, cam_3.distort_t2_);

		file_camera cam_4(4);
		sensor sens_4(4, cam_4.image_size, sensor::projection_pinhole_distort);
		sens_4.set_pinhole_distort(cam_4.world_to_device_pinhole_distort_, cam_4.pinhole_distort_center_, cam_4.pinhole_distort_focus_,
			cam_4.distort_r1_, cam_4.distort_r2_, cam_4.distort_t1_, cam_4.distort_t2_);

		file_camera cam_5(5);
		sensor sens_5(5, cam_5.image_size, sensor::projection_pinhole_distort);
		sens_5.set_pinhole_distort(cam_5.world_to_device_pinhole_distort_, cam_5.pinhole_distort_center_, cam_5.pinhole_distort_focus_,
			cam_5.distort_r1_, cam_5.distort_r2_, cam_5.distort_t1_, cam_5.distort_t2_);

		file_camera cam_6(6);
		sensor sens_6(6, cam_6.image_size, sensor::projection_pinhole_distort);
		sens_6.set_pinhole_distort(cam_6.world_to_device_pinhole_distort_, cam_6.pinhole_distort_center_, cam_6.pinhole_distort_focus_,
			cam_6.distort_r1_, cam_6.distort_r2_, cam_6.distort_t1_, cam_6.distort_t2_);



		//load images used for reconstruction
		
		int width, height;
		int* pixels_0 = image_input::loadPixelGridFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_1.png", width, height);
		int* pixels_1 = image_input::loadPixelGridFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_2.png", width, height);
		int* pixels_2 = image_input::loadPixelGridFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_3.png", width, height);
		int* pixels_3 = image_input::loadPixelGridFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_4.png", width, height);
		int* pixels_4 = image_input::loadPixelGridFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_5.png", width, height);
		int* pixels_5 = image_input::loadPixelGridFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_6.png", width, height);
		int* pixels_6 = image_input::loadPixelGridFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_7.png", width, height);




		std::vector<viral_core::vector> occupiedWorldPositions;

		// run through all positions in world space

		for (int x = -2000; x < 2200; x+=stepsize) {
			for (int y = -2200; y < 2200; y+=stepsize) {
				for (int z = -880; z < 1600; z+=stepsize) {
					
					viral_core::vector vec((float)x, (float)y, (float)z);

					//transform world space position to device positions

					viral_core::vector projVec_0 = sens_0.project(vec);
					viral_core::vector projVec_1 = sens_1.project(vec);
					viral_core::vector projVec_2 = sens_2.project(vec);
					viral_core::vector projVec_3 = sens_3.project(vec);
					viral_core::vector projVec_4 = sens_4.project(vec);
					viral_core::vector projVec_5 = sens_5.project(vec);
					viral_core::vector projVec_6 = sens_6.project(vec);

					//test if device positions are occupied according to loaded images

					int x0 = (int)projVec_0.x;
					int y0 = (int)projVec_0.y;
					int x1 = (int)projVec_1.x;
					int y1 = (int)projVec_1.y;
					int x2 = (int)projVec_2.x;
					int y2 = (int)projVec_2.y;
					int x3 = (int)projVec_3.x;
					int y3 = (int)projVec_3.y;
					int x4 = (int)projVec_4.x;
					int y4 = (int)projVec_4.y;
					int x5 = (int)projVec_5.x;
					int y5 = (int)projVec_5.y;
					int x6 = (int)projVec_6.x;
					int y6 = (int)projVec_6.y;


					bool initial = true;	//if true: world space positions outside of camera viewing frustum assumed occupied, else assumes empty	
					bool set_0 = initial;
					bool set_1 = initial;
					bool set_2 = initial;
					bool set_3 = initial;
					bool set_4 = initial;
					bool set_5 = initial;
					bool set_6 = initial;
					

					if (0 < x0 && x0 < width && 0 < y0 && y0 < height) {
						set_0 = false;
						if (pixels_0[(int)projVec_0.y*width + (int)projVec_0.x] != 0)
							set_0 = true;
					}
					if (0 < x1 && x1 < width && 0 < y1 && y1 < height) {
						set_1 = false;
						if (pixels_1[(int)projVec_1.y*width + (int)projVec_1.x] != 0)
							set_1 = true;
					}
					if (0 < x2 && x2 < width && 0 < y2 && y2 < height) {
						set_2 = false;
						if (pixels_2[(int)projVec_2.y*width + (int)projVec_2.x] != 0)
							set_2 = true;
					}
					if (0 < x3 && x3 < width && 0 < y3 && y3 < height) {
						set_3 = false;
						if (pixels_3[(int)projVec_3.y*width + (int)projVec_3.x] != 0)
							set_3 = true;
					}
					if (0 < x4 && x4 < width && 0 < y4 && y4 < height) {
						set_4 = false;
						if (pixels_4[(int)projVec_4.y*width + (int)projVec_4.x] != 0)
							set_4 = true;
					}
					if (0 < x5 && x5 < width && 0 < y5 && y5 < height) {
						set_5 = false;
						if (pixels_5[(int)projVec_5.y*width + (int)projVec_5.x] != 0)
							set_5 = true;
					}
					if (0 < x6 && x6 < width && 0 < y6 && y6 < height) {
						set_6 = false;
						if (pixels_6[(int)projVec_6.y*width + (int)projVec_6.x] != 0)
							set_6 = true;
					}

					

					//if occupied in every image: set world space position as occupied

					if (set_0 && set_1 && set_2 && set_3 && set_4 && set_5 && set_6) {
						occupiedWorldPositions.emplace_back(vec);
					}
				}
			}
		}
		

		delete [] pixels_0;
		delete [] pixels_1;
		delete [] pixels_2;
		delete [] pixels_3;
		delete [] pixels_4;
		delete [] pixels_5;
		delete [] pixels_6;


		return occupiedWorldPositions;

	}

}