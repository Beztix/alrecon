#ifndef REC_RENDERING
#define REC_RENDERING

#include <vector>
#include <viral_core/geo_vector.hpp>
#include "rec_sensor.h"
#include "rec_file_camera.h"
#include "rec_primitive3D.h"

namespace rec {
	
	void renderWorkspace(rec::aabb workspace, std::vector<rec::file_camera> cameras, std::vector<rec::sensor> sensors, 
		std::vector<std::vector<viral_core::vector>> occupiedPositions, float cubesize, float scale);

}



#endif /*REC_RENDERING*/
