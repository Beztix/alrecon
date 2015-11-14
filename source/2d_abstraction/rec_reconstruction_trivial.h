#ifndef REC_RECONSTRUCTION_TRIVIAL_H
#define REC_RECONSTRUCTION_TRIVIAL_H


#include <vector>
#include <viral_core/geo_3d.hpp>
#include "rec_sensor.h"

namespace rec {

	std::vector<viral_core::vector> reconstruct_trivial(int stepsize, std::vector<rec::sensor> sensors);

}



#endif /*REC_RECONSTRUCTION_TRIVIAL_H*/
