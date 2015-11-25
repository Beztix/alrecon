#ifndef REC_RENDERING_UTIL
#define REC_RENDERING_UTIL

#include <vector>

// Viral
#include <viral_core/engine.hpp>
#include <viral_core/geo_3d.hpp>
#include <viral_core/render_resource.hpp>


namespace rec {
	
	void addLine(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh, 
		viral_core::vector vec_one, viral_core::vector vec_two);


	void addSquare(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh,
		viral_core::vector top_left, viral_core::vector top_right, viral_core::vector bot_left, viral_core::vector bot_right);


	void addCubeAroundVector(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh, viral_core::vector vec, float size);


	void addBoxWithCorners(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh,
		viral_core::vector near_top_left, viral_core::vector near_top_right, viral_core::vector near_bot_left, viral_core::vector near_bot_right,
		viral_core::vector far_top_left, viral_core::vector far_top_right, viral_core::vector far_bot_left, viral_core::vector far_bot_right);

	void addPyramidWithCorners(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh, viral_core::vector apex,
		viral_core::vector bot_left, viral_core::vector bot_right, viral_core::vector top_right, viral_core::vector top_left);


	void addCoordinateSystem(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh, float sizeX, float sizeY, float sizeZ);
}



#endif /*REC_RENDERING_UTIL*/
