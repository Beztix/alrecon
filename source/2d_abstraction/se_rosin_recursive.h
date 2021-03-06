#ifndef SE_ROSIN_RECURSIVE_H
#define SE_ROSIN_RECURSIVE_H

#include <vector>
#include "se_util.h"


/**
* uses the rosin fitting of superellipses in a recursive way to fit multiple superellipses to a contour until a certain quality level is reached
*/
std::vector<se::superellipse> useRosinRecursive(std::vector<cv::Point> contourPoints, int offsetX, int offsetY, const int width, const int height, const int qualityValue);




/**
* this method seperates the given contours and resizes the image to the contour dimensions, then calls useRosinRecursive() on each contour 
*/
std::vector<se::superellipse> startRosinRecursive(std::vector<std::vector<cv::Point>> contours, int &width, int &height, int quality);



#endif /* SE_ROSIN_RECURSIVE_H*/