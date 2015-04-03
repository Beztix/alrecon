#ifndef SE_SPLIT_H
#define SE_SPLIT_H

#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


void splitImageToGrids(int* pixelGrid, const int width, const int height, double xc, double yc, double theta, std::vector<int> &splitPart1, std::vector<int> &splitPart2);

void splitImageToMats(int* pixelGrid, const int width, const int height, double xc, double yc, double theta, cv::Mat &splitPart1, cv::Mat &splitPart2);

void getContours(std::string inputFile, std::string contourFile);

void getContoursRonin(std::string inputFile, std::string contourFile);


#endif /* SE_SPLIT_H*/
