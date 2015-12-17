#ifndef REC_TEST_H
#define REC_TEST_H

#include <viral_core/geo_vector.hpp>

#include <vector>
#include "tree.hh"
#include "rec_containers.h"
#include "rec_sensor.h"


namespace rec {

	
	std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> testPyramidsWithGivenPoint(viral_core::vector p);

	std::vector<std::vector<viral_core::vector>> testReconstruct(int stepsize, std::vector<rec::sensor> sensors, std::vector<std::vector<tree<rec::seAndPyramid>::pre_order_iterator>> intersectionIT);

}


#endif /*REC_TEST_H*/
