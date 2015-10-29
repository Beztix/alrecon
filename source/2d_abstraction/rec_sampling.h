#ifndef REC_SAMPLING
#define REC_SAMPLING

#include "rec_file_camera.h"
#include "rec_sensor.h"

#include <vector>

namespace rec {

	std::vector<viral_core::vector> sample_camera_to_image(file_camera f, sensor s, float distance);

	std::vector<std::vector<viral_core::vector>> sample_camera_for_inv_projection(file_camera f, sensor s, float distance);

	std::vector<std::vector<viral_core::vector>> calculateNormalizedDirectionvectors(viral_core::vector camPosition, int distance, int index, int sizeX, int sizeY);

	void doAllSamplingCalculations();
}



#endif /*REC_SAMPLING*/
