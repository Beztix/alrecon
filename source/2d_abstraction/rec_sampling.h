#ifndef REC_SAMPLING
#define REC_SAMPLING

#include "file_camera.h"
#include "sensor.h"

#include <vector>

namespace rec {

	std::vector<viral_core::vector> sample_camera(file_camera f, sensor s, int index);

}



#endif /*REC_SAMPLING*/
