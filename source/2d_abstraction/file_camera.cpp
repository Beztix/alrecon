/**
*************************************************************************
*
* @file file_camera.cpp
*
* 
*
************************************************************************/

#include "file_camera.h"

#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>


using namespace std;


file_camera::file_camera
(int index)
:
index(index), 
distort_r1_(0), distort_r2_(0),
distort_t1_(0), distort_t2_(0)
{
	//init_occlusion_mask();
	init_calibration("../../assets/TeletubbiesStefan/calibrations/cam");
}



void file_camera::init_calibration(string calibration_base_path)
{
	
	string calibration_path = calibration_base_path + std::to_string(index + 1) + ".cal";

	cout << "Loading calibration data from: " + calibration_path << endl;

	ifstream infile(calibration_path);
	float line_lists[5][4];
	for (int i = 0; i < 5; i++) {
		infile >> line_lists[i][0] >> line_lists[i][1] >> line_lists[i][2] >> line_lists[i][3];
	}

	image_size = viral_core::vector2i(line_lists[3][0], line_lists[3][1]);
	
	// Read full projection matrix, then decompose
	// into rotation, position, calibration matrix.
	double cv_projection_mem[3][4];
	for (int y = 0; y < 3; ++y)
		for (int x = 0; x < 4; ++x)
		{
			world_to_device_pinhole_distort_.c[y * 4 + x] = line_lists[y][x];
			cv_projection_mem[y][x] = line_lists[y][x];
		}


	
	CvMat cv_projection = cvMat(3, 4, CV_64F, cv_projection_mem);
	CvMat *cv_calibration = cvCreateMat(3, 3, CV_64F);
	CvMat *cv_rotation = cvCreateMat(3, 3, CV_64F);
	CvMat *cv_position = cvCreateMat(4, 1, CV_64F);

	
	cvDecomposeProjectionMatrix(&cv_projection, cv_calibration, cv_rotation, cv_position);


	// Read back from OpenCV into slim matrix class.
	// Rescale to fit current scene scale.
	viral_core::matrix calibrate
		((float)cv_calibration->data.db[0], (float)cv_calibration->data.db[1], (float)cv_calibration->data.db[2], 0,
		(float)cv_calibration->data.db[3], (float)cv_calibration->data.db[4], (float)cv_calibration->data.db[5], 0,
		(float)cv_calibration->data.db[6], (float)cv_calibration->data.db[7], (float)cv_calibration->data.db[8], 0,
		0, 0, 0, 1);

	viral_core::matrix rotate
		((float)cv_rotation->data.db[0], (float)cv_rotation->data.db[1], (float)cv_rotation->data.db[2], 0,
		(float)cv_rotation->data.db[3], (float)cv_rotation->data.db[4], (float)cv_rotation->data.db[5], 0,
		(float)cv_rotation->data.db[6], (float)cv_rotation->data.db[7], (float)cv_rotation->data.db[8], 0,
		0, 0, 0, 1);

	viral_core::vector pos
		((float)cv_position->data.db[0] / (float)cv_position->data.db[3],
		(float)cv_position->data.db[1] / (float)cv_position->data.db[3],
		(float)cv_position->data.db[2] / (float)cv_position->data.db[3]);


	// Apply all camera parameters.
	// world_to_device_pinhole_distort_ = calibrate * rotate * matrix::translate(pos);

	distort_r1_ = line_lists[4][0];
	distort_r2_ = line_lists[4][1];
	distort_t1_ = line_lists[4][2];
	distort_t2_ = line_lists[4][3];

	pinhole_distort_focus_ = viral_core::vector2f(calibrate.c[0] / calibrate.c[10], calibrate.c[5] / calibrate.c[10]);
	pinhole_distort_center_ = viral_core::vector2f(calibrate.c[2] / calibrate.c[10], calibrate.c[6] / calibrate.c[10]);

	cam_position_ = pos;
	cam_orientation_ = viral_core::rotation(rotate.x_axis(), rotate.y_axis(), rotate.z_axis());


	cvReleaseMat(&cv_calibration);
	cvReleaseMat(&cv_rotation);
	cvReleaseMat(&cv_position);
}

