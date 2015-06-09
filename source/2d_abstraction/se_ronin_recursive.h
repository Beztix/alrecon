#ifndef SE_RONIN_RECURSIVE_H
#define SE_RONIN_RECURSIVE_H

#include <vector>

std::vector<std::vector<double>> useRoninRecursive(std::vector<cv::Point> contourPoints, int offsetX, int offsetY, const int width, const int height, const int qualityValue);


#endif /* SE_RONIN_RECURSIVE_H*/