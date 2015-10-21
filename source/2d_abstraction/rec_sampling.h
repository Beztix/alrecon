#ifndef REC_SAMPLING
#define REC_SAMPLING

#include "rec_file_camera.h"
#include "rec_sensor.h"

#include <vector>

namespace rec {

	std::vector<viral_core::vector> sample_camera_to_image(file_camera f, sensor s);

	std::vector<std::vector<viral_core::vector>> sample_camera_for_inv_projection(file_camera f, sensor s);

}



#endif /*REC_SAMPLING*/
