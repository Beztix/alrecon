#ifndef REC_RENDERING
#define REC_RENDERING

#include <vector>
#include <viral_core/geo_3d.hpp>
#include "rec_sensor.h"
#include "rec_file_camera.h"

namespace rec {
	
	void renderWorkspace(std::vector<rec::file_camera> cameras, std::vector<rec::sensor> sensors, 
		std::vector<std::vector<viral_core::vector>> occupiedPositions, float cubesize, float scale);

}



#endif /*REC_RENDERING*/
