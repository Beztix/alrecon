#ifndef IMAGE_INPUT_H
#define IMAGE_INPUT_H

#include <iostream>


namespace image_input {

	int* loadPixelsFromImage(std::string imageLocation, int &width, int &height);


	int* reducePixels(int* pixels, int width, int height, int spacing, std::string reducedImgName);

	void renderImage(std::string name, int width, int height, int num, double data[1000]);

}


#endif /* IMAGE_INPUT_H*/
