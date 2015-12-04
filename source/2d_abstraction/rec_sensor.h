#ifndef REC_SENSOR_H
#define REC_SENSOR_H


#include <viral_core/geo_vector.hpp>
#include <viral_core/geo_transform.hpp>
#include <viral_core/geo_2d.hpp>


namespace rec {

	class sensor
	{
	public:

		enum projection_mode
		{
			projection_camera,
			projection_pinhole_distort,
			projection_separate_xy_depth
		};


		sensor(int index, const viral_core::vector2i& image_size,
			projection_mode projection);


		void set_pinhole_distort
			(const viral_core::matrix& world_to_device_pinhole_distort,
			const viral_core::vector2f& pinhole_distort_center,
			const viral_core::vector2f& pinhole_distort_focus,
			float distort_r1, float distort_r2,
			float distort_t1, float distort_t2);

		const int index;
		const viral_core::vector2i image_size;
		const projection_mode projection;


		viral_core::vector project(const viral_core::vector& v) const;


	private:

		viral_core::matrix world_to_device_;

		viral_core::matrix world_to_device_pinhole_distort_;
		viral_core::vector2f pinhole_distort_center_;
		viral_core::vector2f pinhole_distort_focus_;
		viral_core::vector2f pinhole_distort_focus_inv_;
		float distort_r1_, distort_r2_, distort_t1_, distort_t2_;

		viral_core::matrix world_to_device_xy_;
		viral_core::matrix world_to_device_depth_;


	};

}


#endif /*REC_SENSOR_H*/
