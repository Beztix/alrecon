#ifndef INPUT_H
#define INPUT_H

#include <iostream>
namespace input {

	int* loadPixelsFromImage(std::string imageLocation, int *widthPtr, int *heightPtr);


	int* reducePixels(int* pixels, int width, int height, int spacing);

}


#endif /* INPUT_H*/
