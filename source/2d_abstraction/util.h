#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <opencv2/core/core.hpp>

namespace util {


	void sortPixelFile(std::string input, std::string output);

	std::vector<int> reducePixels(int* pixels, int width, int height, int spacing, std::string reducedImgName);

	std::vector<int> getContourVector(std::vector<int> inputPixelGrid, int width, int height);
	
	int* getFilledPixelGridFromContour(std::vector<cv::Point> contourPoints, int width, int height);

	std::vector<cv::Point> bresenhamLine(const cv::Point p1, const cv::Point p2);

	int testSegmentsIntersection(cv::Point a1, cv::Point a2, cv::Point b1, cv::Point b2);

	cv::Point2d intersectLines(cv::Point a1, cv::Point a2, cv::Point b1, cv::Point b2);


	void getContours(std::string inputFile, std::string contourFile);

	void getContoursRonin(std::string inputFile, std::string contourFile);

}


#endif /* UTIL_H*/
