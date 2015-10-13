#ifndef FILE_CAMERA_H
#define FILE_CAMERA_H

#include <string>

#include <viral_core/geo_3d.hpp>
#include <viral_core/geo_2d.hpp>


class file_camera
{

	const int index;

	file_camera (int index);

	void init_calibration(std::string calibration_base_path);

	viral_core::matrix world_to_device_pinhole_distort_;

	float distort_r1_, distort_r2_;
	float distort_t1_, distort_t2_;

	viral_core::vector2f pinhole_distort_center_;
	viral_core::vector2f pinhole_distort_focus_;

	viral_core::vector cam_position_;
	viral_core::rotation cam_orientation_;
};



#endif /*FILE_CAMERA_H*/
