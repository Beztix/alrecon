#ifndef SE_SPLIT_H
#define SE_SPLIT_H

#include <iostream>
#include <string>


void splitImage(int* pixelGrid, const int width, const int height, double xc, double yc, double theta, std::vector<int> &splitPart1, std::vector<int> &splitPart2);

void getContours(std::string inputFile, std::string contourFile);

void getContoursRonin(std::string inputFile, std::string contourFile);

std::vector<int> getContoursArray(int* inputPixelGrid, int width, int height);

#endif /* SE_SPLIT_H*/
