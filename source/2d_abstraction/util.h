#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <opencv2/core/core.hpp>

namespace util {


	void sortPixelFile(std::string input, std::string output);

	std::vector<int> reducePixels(int* pixels, int width, int height, int spacing, std::string reducedImgName);

	std::vector<int> getContourVector(std::vector<int> inputPixelGrid, int width, int height);
	
	int* getFilledPixelGridFromContour(std::vector<cv::Point> contourPoints, int width, int height);

}


#endif /* UTIL_H*/
