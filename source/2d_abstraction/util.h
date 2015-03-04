#ifndef UTIL_H
#define UTIL_H

#include <string>

namespace util {


	void sortPixelFile(std::string input, std::string output);

	int* reducePixels(int* pixels, int width, int height, int spacing, std::string reducedImgName);
	
}


#endif /* UTIL_H*/
