#ifndef FILE_CAMERA_H
#define FILE_CAMERA_H


class file_camera
{

	const int index;

	file_camera (int index);

	void init_calibration(string calibration_base_path);

	matrix world_to_device_pinhole_distort_;

	float distort_r1_, distort_r2_;
	float distort_t1_, distort_t2_;

	vector2f pinhole_distort_center_;
	vector2f pinhole_distort_focus_;

	vector cam_position_;
	rotation cam_orientation_;
};



#endif /*FILE_CAMERA_H*/
