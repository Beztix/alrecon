#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <opencv2/core/core.hpp>

namespace util {


	struct float3 {
		float x;
		float y;
		float z;

		float3() {
			x = 0;
			y = 0;
			z = 0;
		}

		float3(float mx, float my, float mz) {
			x = mx;
			y = my;
			z = mz;
		}

		float3 operator+(const float3 f) {
			return float3(x + f.x, y + f.y, z + f.z);
		}

		float3 operator-(const float3 f) {
			return float3(x - f.x, y - f.y, z - f.z);
		}

		float3 operator*(const float f) {
			return float3(x*f, y*f, z*f);
		}
	};



	void sortPixelFile(std::string input, std::string output);

	std::vector<int> reducePixels(int* pixels, int width, int height, int spacing, std::string reducedImgName);

	std::vector<int> getContourVector(std::vector<int> inputPixelGrid, int width, int height);
	
	int* getFilledPixelGridFromContour(std::vector<cv::Point> contourPoints, int width, int height);

	std::vector<cv::Point> bresenhamLine(const cv::Point p1, const cv::Point p2);

	int testSegmentsIntersection(cv::Point a1, cv::Point a2, cv::Point b1, cv::Point b2);

	cv::Point2d intersectLines(cv::Point a1, cv::Point a2, cv::Point b1, cv::Point b2);


	void getContours(std::string inputFile, std::string contourFile);

	void getContoursRonin(std::string inputFile, std::string contourFile);


	float3 ClosestPointLineLine(float3 start0, float3 end0, float3 start1, float3 end1);

}


#endif /* UTIL_H*/
