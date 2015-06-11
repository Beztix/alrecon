#ifndef SE_SPLIT_H
#define SE_SPLIT_H

#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

/// This method splits an image given as a pixelGrid by using a split line to two vectors containing the coordinates of the points
void splitImagegridToCoordinatevectors(int* pixelGrid, const int width, const int height, double xc, double yc, double theta, std::vector<int> &splitPart1, std::vector<int> &splitPart2);


/// This method splits an image given as a pixelGrid by using a split line to two Mats
void splitImagegridToMats(std::vector<int> pixelGrid, const int width, const int height, double xc, double yc, double theta, cv::Mat &splitPart1, cv::Mat &splitPart2);


/// This method splits an contour given as a vector of cv::Point by using a split line to two new contours given as vectors of cv::Point
void splitContourToVectors(std::vector<cv::Point> contourPoints, const int width, const int height, const double xc, const double yc, const double theta, std::vector<cv::Point> &splitPart1, std::vector<cv::Point> &splitPart2);


#endif /* SE_SPLIT_H*/
