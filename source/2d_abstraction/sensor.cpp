/**
*************************************************************************
*
* @file sensor.cpp
*
* 
*
************************************************************************/

#include "sensor.h"


sensor::sensor
(int index, const viral_core::vector2i& image_size,
projection_mode projection)
:
index(index),
image_size(image_size),
projection(projection),
distort_r1_(0), distort_r2_(0),
distort_t1_(0), distort_t2_(0)
{ }




void sensor::set_pinhole_distort
(const viral_core::matrix& world_to_device_pinhole_distort,
const viral_core::vector2f& pinhole_distort_center,
const viral_core::vector2f& pinhole_distort_focus,
float distort_r1, float distort_r2,
float distort_t1, float distort_t2)
{
	world_to_device_pinhole_distort_ = world_to_device_pinhole_distort;
	pinhole_distort_center_ = pinhole_distort_center;
	pinhole_distort_focus_ = pinhole_distort_focus;
	distort_r1_ = distort_r1;
	distort_r2_ = distort_r2;
	distort_t1_ = distort_t1;
	distort_t2_ = distort_t2;

	pinhole_distort_focus_inv_ =
		viral_core::vector2f(1.f / pinhole_distort_focus.x,
							 1.f / pinhole_distort_focus.y);
}





viral_core::vector sensor::project(const viral_core::vector& v) const
{
	if (projection == projection_separate_xy_depth)
	{
		float z = world_to_device_depth_.project(v).z;
		viral_core::vector xy(world_to_device_xy_.project(v));
		return viral_core::vector(xy.x, xy.y, z);
	}
	else if (projection == projection_pinhole_distort)
	{
		const viral_core::matrix& w2d = world_to_device_pinhole_distort_;

		float z = (w2d.c20 * v.x + w2d.c21 * v.y + w2d.c22 * v.z + w2d.c23);
		float inv_z = 1 / z;
		float ud_x = (w2d.c00 * v.x + w2d.c01 * v.y + w2d.c02 * v.z + w2d.c03) * inv_z;
		float ud_y = (w2d.c10 * v.x + w2d.c11 * v.y + w2d.c12 * v.z + w2d.c13) * inv_z;

		/*
		TODO Warning this is required for Stefan's projections,
		which already include center and focus distortions in the world to device matrix.
		*/
		ud_x = (ud_x - pinhole_distort_center_.x) * pinhole_distort_focus_inv_.x;
		ud_y = (ud_y - pinhole_distort_center_.y) * pinhole_distort_focus_inv_.y;
		

		float rad_2 = ud_x * ud_x + ud_y * ud_y;
		float rad_4 = rad_2 * rad_2;

		float distorted_x =
			(1 + distort_r1_ * rad_2 + distort_r2_ * rad_4) * ud_x +
			2 * distort_t1_ * ud_x * ud_y +
			distort_t2_ * (rad_2 + 2 * ud_x * ud_x);
		float distorted_y =
			(1 + distort_r1_ * rad_2 + distort_r2_ * rad_4) * ud_y +
			2 * distort_t2_ * ud_x * ud_y +
			distort_t1_ * (rad_2 + 2 * ud_y * ud_y);

		distorted_x =
			distorted_x * pinhole_distort_focus_.x + pinhole_distort_center_.x;
		distorted_y =
			distorted_y * pinhole_distort_focus_.y + pinhole_distort_center_.y;

		return viral_core::vector
			(distorted_x + 0.5f, distorted_y + 0.5f, z);
	}
	else if (projection == projection_camera)
	{
		viral_core::vector projection
			(world_to_device_.project(v));

		return viral_core::vector
			((projection.x + 1) * 0.5f * image_size.x,
			(-projection.y + 1) * 0.5f * image_size.y,
			projection.z);
	}



	return viral_core::vector();
}

