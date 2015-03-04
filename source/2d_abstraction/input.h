#ifndef INPUT_H
#define INPUT_H

#include <iostream>
namespace input {

	int* loadPixelsFromImage(std::string imageLocation, int *widthPtr, int *heightPtr);


	int* reducePixels(int* pixels, int width, int height, int spacing, std::string reducedImgName);

	void renderImage(std::string name, int width, int height, int num, double data[1000]);

}


#endif /* INPUT_H*/
